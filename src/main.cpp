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

//Library
#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <timeGSM.h>
#include <WiFi.h>
#include <tool.h>
#include <eepromESP.h>
#include <eepromESP_table.h>
#include <TimeLib.h>
#include <TimeAlarms.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

timeGSM chrono;
eepromESP flash;
slave slave;

AlarmId on_alarm, 
        off_alarm, 
        dailysync_alarm;

const String dev_type = "slg/";
String dev_code       = "";
String apn            = "";
String backend        = "";
String backend_user   = "";
String backend_pass   = "";
uint16_t backend_port = 0;


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

uint8_t now_hour = 0, 
        now_minute = 0, 
        now_second = 0, 
        now_date =0, 
        now_month = 0;

uint16_t now_year = 0;
uint8_t timer_update = 0;
uint8_t timer_request = 0;
bool command_lamp = 0;

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
void update_data();
void adjust_time(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second);
void change_schedule(bool change_mode);
void cek_lampstate();

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

  modem.restart();
  
  chrono.TimeInit();
  cek_time();
  delay(100);
  
  if(chrono.getYear()<2010){
    chrono.getGSMtime(modem.getGSMDateTime(DATE_FULL));
    Serial.println("sycn time with sim800l ");
  }         
  RTOS_initialization();
  
}

void loop() {
  //empty
}

void RTOS_initialization(){
   xTaskCreatePinnedToCore(
    task_connectivity
    ,  "task_connectivity"   // A name just for humans
    ,  10000  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    task_lamp
    ,  "task_lamp"
    ,  10000  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
}

void task_connectivity(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
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
    delay(15);  // one tick delay (15ms) in between reads for stability
  }
}

void task_lamp(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
    cek_time();
    if(mode == AUTOMATIC_MODE){
      on_alarm = Alarm.alarmRepeat(on_hour,on_minute,0, control_on);
      off_alarm = Alarm.alarmRepeat(off_hour,off_minute,0, control_off);
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
    Alarm.delay(10);
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
  // Serial.println((String)now_hour+":"
  //               +(String)now_minute+":"
  //               +(String)now_second+" "
  //               +(String)now_date+"/"
  //               +(String)now_month+"/"
  //               +(String)now_year);
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
  String pub_data = String(wh)+","+String(pow)+","+String(volt)+","+String(curr)+","+String(pf);
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
  slave.send_command(LAMP_ON);
}
void control_off(){
  slave.send_command(LAMP_OFF);
}
void update_data(){
  slave.request_data();
}

void adjust_time(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second){
  chrono.adjust_rtc(year,month,date,hour,minute,second);
  Serial.println("time ajust " + (String)year + (String)month + (String)date + "-" 
                  + (String)hour + (String)minute + (String)second);
}

void change_schedule(bool change_mod){
  //Serial.println("schedule change");
  Alarm.free(on_alarm);
  on_alarm = dtINVALID_ALARM_ID;
  Alarm.free(off_alarm);
  off_alarm = dtINVALID_ALARM_ID;
  delay(1000);

  if(change_mod == AUTOMATIC_MODE){
    on_alarm = Alarm.alarmRepeat(on_hour,on_minute,0, control_on);
    off_alarm = Alarm.alarmRepeat(off_hour,off_minute,0, control_off);
  }
  else if(change_mod == MANUAL_MODE){
    slave.send_command(ctrl);
    delay(50);
  }
  cek_lampstate();
}

void cek_lampstate(){
  unsigned long cur_time = ((uint32_t)chrono.getHour()*3600) + ((uint32_t)chrono.getMinute()*60) + ((uint32_t)chrono.getSecond());
  unsigned long on_time = (on_hour*3600) + (on_minute*60);
  unsigned long off_time = (off_hour*3600) + (off_minute*60);
  
  if(mode == AUTOMATIC_MODE)
  {
    if((int)(on_hour-off_hour)>0) 
      {
          if((cur_time>=on_time && cur_time>=off_time) || (cur_time<=off_time && cur_time<=on_time))
          {
            control_on();
            Serial.println("on1");
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
          }
        }
  }
  else if(mode == MANUAL_MODE)
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
}

