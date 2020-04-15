//version
#define FW_VERSION "1.0"
#define MODEL "8 W/ OTA"

//FreeRTOS
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//SIM800_Init
#define TINY_GSM_MODEM_SIM800
#define SerialAT  Serial2

//lamp type
#define SONT 0
#define LED  1

#define LAMP_ON 1
#define LAMP_OFF 0

//Mqtt_channel
#define PUBS_TOPIC  dev_type + "rs/" + dev_code
#define HB_TOPIC    dev_type + "hb/" + dev_code
#define DATA_TOPIC  dev_type + "rc/" + dev_code
#define HEAD_TOPIC  dev_type + dev_code
#define SUBS_TOPIC  dev_type + dev_code + "/#"

#define MANUAL_MODE 0
#define AUTOMATIC_MODE 1
#define NIGHT_MODE 2

//Library
#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <timeGSM.h>
#include <tool.h>
#include <eepromESP.h>
#include <eepromESP_table.h>
#include <TimeAlarms.h>
#include <OTA_manager.h>
#include <CRC32.h>
#include <WiFi.h>
#include <index_html.h>
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <ESPmDNS.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
update_manager OTA_manager;

timeGSM chrono;
eepromESP flash;
slave slave;
AsyncWebServer myserver(80);
size_t content_len;

AlarmId on_alarm, 
        off_alarm, 
        night_alarm,
        dailysync_alarm;

const String dev_type = "slg/";
String dev_code       = "";
String apn            = "";
String backend        = "";
String backend_user   = "";
String backend_pass   = "";
uint16_t backend_port = 0;

String ota_server = "";
uint16_t ota_port = 0;
String ota_resource = "";

uint8_t netfail_counter = 0;
uint8_t mode = 0;
uint8_t ctrl = 0;
uint16_t timedelay = 0;
uint32_t  on_hour = 0, 
          on_minute = 0, 
          on_second = 0, 
          off_hour = 0,
          off_minute = 0, 
          off_second = 0,
          night_hour = 0,
          night_minute = 0;

uint8_t dimmer1 = 0;
uint8_t dimmer2 = 0;
uint16_t lamppower = 0;

uint8_t now_hour = 0, 
        now_minute = 0, 
        now_second = 0, 
        now_date =0, 
        now_month = 0;

uint16_t now_year = 0;
uint8_t timer_update = 0;
uint8_t timer_request = 0;

bool command_lamp = false;
bool lamp_state = false;
bool task_update = false;

String ssid = "CUBE";
String password = "123456789";
char host[25] = "slighter_dashboard";

//RTOS_func_declaration
void task_connectivity( void *pvParameters );
void task_lamp( void *pvParameters );
void RTOS_initialization();

//function_definition
void gsmConnect();
void reconnect();
void mqttCallback(char* topic, byte* payload, unsigned int len);
void gsm_timeInit();
void cek_time();
void data_process(float volt, float curr, float pow, float wh, float freq, float pf);
void control_on();
void control_off();
void control_dimmer();
void update_data();
void adjust_time(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second);
void change_schedule(uint8_t change_mod);
void cek_lampstate();
void ceate_OTAtask();
void OTA_task( void * parameter );
String processor(const String& var);
void start_webserver();
void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
void printProgress(size_t prg, size_t sz);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialAT.begin(115200);
  delay(1000);
  Serial.println("ESP32 RESET SYSTEM");
  flash.begin(ee_size);
  config_all();
  dev_code = dev_id();
  Serial.println("Device code : " + dev_code);
  Serial.print("Subs ch : "); Serial.println(SUBS_TOPIC);

  chrono.TimeInit();

  start_webserver();

  modem.restart();
  
  cek_time();
  delay(100);
  
  if(chrono.getYear()<2010){
    chrono.getGSMtime(modem.getGSMDateTime(DATE_FULL));
    Serial.println("sycn time with sim800l ");
  }         
  RTOS_initialization();
}

void loop() {}

void RTOS_initialization(){
   xTaskCreatePinnedToCore(
    task_connectivity
    ,  "task_connectivity"   // A name just for humans
    ,  30000  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    task_lamp
    ,  "task_lamp"
    ,  20000  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
}

void task_connectivity(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.println("task connectivity");
  gsmConnect();
  mqtt.setServer(backend.c_str(), backend_port);
  mqtt.setCallback(mqttCallback);
  reconnect();

  for (;;)
  {
    if(!mqtt.loop())
    {
      reconnect();
    }
    vTaskDelay(1); // one tick delay (15ms) in between reads for stability
  }
}

void task_lamp(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
    cek_time();
    if(mode != MANUAL_MODE){
      on_alarm = Alarm.alarmRepeat(on_hour,on_minute,0, control_on);
      off_alarm = Alarm.alarmRepeat(off_hour,off_minute,0, control_off);
      if(mode != NIGHT_MODE){
        night_alarm = Alarm.alarmRepeat(night_hour,night_minute,0,control_dimmer);  
      }
    }
    else{
      if(ctrl == LAMP_ON){
        control_on();}
      else{
        control_off();}
    }
    dailysync_alarm = Alarm.alarmRepeat(16,35,0, cek_time);
    Alarm.timerRepeat(timedelay*60, update_data);
      
  for (;;)
  {
    serial_handle();
    Alarm.delay(1000);
    vTaskDelay(1);
    if(task_update){
      Serial.println("lamp task suspended");
      vTaskDelete( NULL );
    }
  }
}

void gsmConnect(){
  if(netfail_counter>=3)
  {
    Serial.println("ESP reset caused network unvailable");
    ESP.restart();
  }
  modem.restart();
  Serial.println("Modem: " + modem.getModemInfo());
  Serial.println("Search");
  if(!modem.waitForNetwork())
  {
    netfail_counter++;
    Serial.println("netfail.."+(String)netfail_counter);
    gsmConnect();
  }
  netfail_counter = 0;
  Serial.println("Signal:" + String(modem.getSignalQuality()));
  Serial.print("Connect GPRS...");
  if (!modem.gprsConnect(apn.c_str(), NULL, NULL))
  {
    Serial.println("gprsfail");
    gsmConnect();
  }
  Serial.println("netconnect");
}

void reconnect() {
  uint8_t retry_error = 0;
  while (!mqtt.connected()) {
    if(!modem.isNetworkConnected() || retry_error>=5)
    {
      gsmConnect();
    }
    Serial.print("Attempting... ");
    if(mqtt.connect(dev_code.c_str(),backend_user.c_str(),backend_pass.c_str())){
      Serial.println("mqttconnect");
      String sub = SUBS_TOPIC;
      mqtt.subscribe(sub.c_str());
      retry_error = 0;
    }
    else {
      Serial.print("mqttfail=");
      Serial.print(mqtt.state());
      retry_error++;
      Serial.print("rtry: "); Serial.println(retry_error);
    }
    if(retry_error>=5){
      retry_error = 0;
      gsmConnect();
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int len){
  String topic_s = topic;
  String payload_s = (char*)payload;
  payload_s = payload_s.substring(0,len);
  String pub = PUBS_TOPIC;
  if(topic_s.substring(0,10) == HEAD_TOPIC){
    String return_message = callback_handle(topic_s.substring(10),payload_s);
    mqtt.publish(pub.c_str(), return_message.c_str());
  }
}

void cek_time(){
  now_hour = chrono.getHour(); delay(2);
  now_minute = chrono.getMinute(); delay(2);
  now_second = chrono.getSecond(); delay(2);
  now_date = chrono.getDate(); delay(2);
  now_month = chrono.getMonth(); delay(2);
  now_year = chrono.getYear(); delay(2);
  Serial.println((String)now_hour+":"
                +(String)now_minute+":"
                +(String)now_second+" "
                +(String)now_date+"/"
                +(String)now_month+"/"
                +(String)now_year);
  setTime(
          now_hour
          ,now_minute
          ,now_second
          ,now_month
          ,now_date
          ,now_year-2000);

  cek_lampstate();
}

void gsm_timeInit(){
  SerialAT.println("AT+CLTS=1;&W");
  modem.restart();
}

void data_process(float volt, float curr, float pow, float wh, float freq, float pf){
  if(pow >= (0.5*lamppower)){
    lamp_state = true;
  }
  else{
    lamp_state = false;
  }

  String pub_data = String(wh)+","+String(pow)+","+String(volt)+","+String(curr)+","+String(pf)+","+String(command_lamp)+","+String(lamp_state);
  String pub_topic = DATA_TOPIC;
  
  if(mqtt.connected()){
    mqtt.publish(pub_topic.c_str(),pub_data.c_str());
    delay(10);
    Serial.println("success->publish data");
  }
  else{
    Serial.print("failed->publish data");
    return;
  }
  
}

void control_on(){
  command_lamp = true;
  slave.send_command(LAMP_ON);
}

void control_off(){
  command_lamp = false;
  slave.send_command(LAMP_OFF);
}

void control_dimmer(){
  Serial.println("night mode");
}

void update_data(){
  slave.request_data();
}

void adjust_time(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second){
  chrono.adjust_rtc(year,month,date,hour,minute,second);
  Serial.println("time ajust " + (String)year + (String)month + (String)date + "-" 
                  + (String)hour + (String)minute + (String)second);
}

void change_schedule(uint8_t change_mod){
  //Serial.println("schedule change");
  Alarm.free(on_alarm);
  on_alarm = dtINVALID_ALARM_ID;
  Alarm.free(off_alarm);
  off_alarm = dtINVALID_ALARM_ID;
  delay(1000);
  if(change_mod == MANUAL_MODE){
    slave.send_command(ctrl);
    delay(50);
  }
  else{
    on_alarm = Alarm.alarmRepeat(on_hour,on_minute,0, control_on);
    off_alarm = Alarm.alarmRepeat(off_hour,off_minute,0, control_off);
    if(change_mod == NIGHT_MODE){
      Alarm.free(night_alarm);
      night_alarm = dtINVALID_ALARM_ID;
      night_alarm = Alarm.alarmRepeat(night_hour,night_minute,0, control_dimmer);
    }
  }
  cek_lampstate();
}

void cek_lampstate(){
  unsigned long cur_time = ((uint32_t)chrono.getHour()*3600) + ((uint32_t)chrono.getMinute()*60) + ((uint32_t)chrono.getSecond());
  unsigned long on_time = (on_hour*3600) + (on_minute*60);
  unsigned long off_time = (off_hour*3600) + (off_minute*60);
  unsigned long night_time = (night_hour*3600) + (night_minute*60);
  
  if(mode == MANUAL_MODE)
  {
    if(ctrl==1)
    {
      control_on();
    }
    else
    {
      control_off();
    }
  }
  else
  {
    if((int)(on_hour-off_hour)>0) 
      {
          if((cur_time>=on_time && cur_time>=off_time) || (cur_time<=off_time && cur_time<=on_time))
          {
            control_on();
            Serial.println("on1");
            if(mode == NIGHT_MODE){
              if((cur_time>=night_time) || (cur_time<=night_time && cur_time<=on_time)){
                Serial.println("night mode1 on");
              }
            }
          }
          else
          {
            control_off();
            Serial.println("off1");
          }
       }
      else 
        {
          if(cur_time<=on_time || cur_time>=off_time)
          {
            control_off();
            Serial.println("off2");
          }
          else
          {
            control_on();
            Serial.println("on2");
            if(cur_time>=night_time){
              Serial.println("night mode2 on");
            }
          }
        }
  }
}

void ceate_OTAtask(){

  Serial.print("perform OTA update on core :");
  Serial.println(xPortGetCoreID());
  uint32_t knownCRC32 = 0x6f50d767;
  uint32_t knownFileSize = 1024;
  OTA_manager.spiffs_init();
  yield();
  gsmConnect();
  yield();
  Serial.print("connect OTA server :" + ota_server);
  if (!client.connect(ota_server.c_str(), ota_port))
  {
    Serial.println(" fail connect OTA server");
    delay(10000);
    return;
  }
  Serial.println("connected to OTA server");
    // Make a HTTP request:
  client.print(String("GET ") + ota_resource + " HTTP/1.0\r\n");
  client.print(String("Host: ") + ota_server + "\r\n");
  client.print("Connection: close\r\n\r\n");
  long timeout = millis();
  while (client.available() == 0)
  {
     if (millis() - timeout > 500000L)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(10000L);
      return;
    }
  }

  Serial.println("Receiving Header");
  uint32_t contentLength = knownFileSize;

  File file = SPIFFS.open("/update.bin", FILE_APPEND);

  while (client.available())
    {
        String line = client.readStringUntil('\n');
        line.trim();
        Serial.println(line);    // Uncomment this to show response header
        line.toLowerCase();
        if (line.startsWith("content-length:"))
        {
            contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
        }
        else if (line.length() == 0)
        {
            break;
        }
    }

    Serial.println("Receiving Response");
    timeout = millis();
    uint32_t readLength = 0;
    CRC32 crc;

    unsigned long timeElapsed = millis();
    OTA_manager.printPercent(readLength, contentLength);
    char c;
    while (readLength < contentLength)
    {
        int i = 0;
        while (client.available())
        {
           c =  char(client.read());
            //Serial.print((char)c);
                   // Uncomment this to show data
            if (!file.print(c))
            {
                Serial.println("Append failed");
            }
            //crc.update(c);
            readLength++;
            if (readLength % (contentLength / 13) == 0)
            {
                OTA_manager.printPercent(readLength, contentLength);
            }
            timeout = millis();
        }
    }

    file.close();

    OTA_manager.printPercent(readLength, contentLength);
    timeElapsed = millis() - timeElapsed;
    Serial.println();

    client.stop();
    Serial.println("Disconnected from Server");
    yield();
    modem.gprsDisconnect();
    Serial.println("GPRS Disconnected");
    yield();

    xTaskCreate(OTA_task,"OTA_task",50000,NULL,5,NULL);
    vTaskDelete( NULL );
}

void OTA_task( void * parameter ){
  (void)parameter;

    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;

  for(;;)
  {
    //float duration = float(timeElapsed) / 1000.0;

    // Serial.println("File size: " + contentLength);
    // Serial.println("read : " + readLength);
    // Serial.print("Calculated. CRC32: 0x");
    // Serial.println(crc.finalize(), HEX);
    // Serial.print("known CRC32:    0x");
    // Serial.println(knownCRC32, HEX);
    //Serial.println("Time duration: " + String(duration) + "s");
    Serial.println("Wait for 3 second");
    for (int i = 0; i < 3; i++)
    {
        Serial.print(String(i) + "...");
        delay(1000);
    }

    //readFile(SPIFFS, "/update.bin");

    OTA_manager.updateFromFS();

    // Do nothing forevermore
    // while (true)
    // {
    //     delay(1000);
    // }
  }
}

String processor(const String& var){
  if(var == "APN"){
    return apn.c_str();
  }
  else if(var == "MODEL"){
    return MODEL;
  }
  else if(var == "FW_VERSION"){
    return FW_VERSION;
  }
  else if(var == "SSID"){
    return ssid;
  }
  else if(var == "PASSWRD"){
    return password.c_str();
  }
  else if(var == "PASSWRDDSPLY"){
    int len = password.length();
    String buf = "";
    for(int i=1; i<=len; i++){
      buf += "*";
    }
    return buf;
  }
  else if(var == "PJU_CODE"){
    return dev_code.c_str();
  }
  else if(var == "BACKSERVER"){
    return backend.c_str();
  }
  else if(var == "BACKUSER"){
    return backend_user.c_str();
  }
  else if(var == "BACKPASS"){
    return backend_pass.c_str();
  }
  else if(var == "BACKPASSDSPLY"){
    int len = backend_pass.length();
    String buf = "";
    for(int i=1; i<=len; i++){
      buf += "*";
    }
    return buf;
  }
  else if(var == "BACKPORT"){
    return String(backend_port);
  }
  else if(var == "DEVTIME"){
    return chrono.get_completeTime().c_str();
  }
  return String();
}

void start_webserver(){
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.print("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  MDNS.begin(host);

  myserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
    });

  myserver.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //http://192.168.43.228/get?apn=axis&ssid=CUBE&ssid_password=123456789&backend_server=158.140.167.173&backend_port=1884&backend_username=eyroMQTT&backend_password=eyroMQTT1234
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>    
    if (request->hasParam("apn")) {
      parse_apn(request->getParam("apn")->value());
    }
    if (request->hasParam("ssid") && request->hasParam("ssid_password")){
      String value = request->getParam("ssid")->value() + "|" + request->getParam("ssid_password")->value();
      parse_wifi(value); 
    }
    if (request->hasParam("backend_server") && request->hasParam("backend_username") && request->hasParam("backend_port") && request->hasParam("backend_password")){
      String value = request->getParam("backend_server")->value() + "|"
                + request->getParam("backend_port")->value().toInt() + "|"
                + request->getParam("backend_username")->value() + "|"
                + request->getParam("backend_password")->value();
      parse_server(value);
    }
    request->send(200, "text/html", "Setting parameter success, please return home page and reboot soon! <br><a href=\"/\">Return to Home Page</a>");
  });

  myserver.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){   
    request->send(200, "text/html", "slighter will reboot and take a few minute <br><a href=\"/\">Return to Home Page</a>");
    ESP.restart();
  });

  myserver.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){   
    request->send(200, "text/html", "<form method='POST' action='/doUpdate' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
  });
  
  myserver.on("/doUpdate", HTTP_POST,
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                  size_t len, bool final) {handleDoUpdate(request, filename, index, data, len, final);}
  );

  myserver.on("/resetwh", HTTP_GET, [](AsyncWebServerRequest *request){
    slave.reset_energy();   
    request->send(200, "text/html", "Reset WH success <br><a href=\"/\">Return to Home Page</a>");
  });

  Update.onProgress(printProgress);
  myserver.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.printf("ready! open http://%s in your browser", host);
}

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    Serial.println("Update");
    content_len = request->contentLength();
    // if filename includes spiffs, update the spiffs partition
    int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }

  if (final) {
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
    response->addHeader("Refresh", "20");  
    response->addHeader("Location", "/");
    request->send(response);
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      Serial.println("Update complete");
      Serial.flush();
      ESP.restart();
    }
  }
}

void printProgress(size_t prg, size_t sz) {
  Serial.printf("Progress: %d%%\n", (prg*100)/content_len);
}
