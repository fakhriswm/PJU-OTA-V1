/*
 * tool.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: luqma
 */


#include "tool.h"
#include <confmanager.h>


#define TOKEN_LENGTH 6

confmanager config_t;
time_on_config_t time_on_cfg;
time_off_config_t time_off_cfg;
night_time_config_t night_time_cfg;

slave Slave;


String get_mac_address(bool delimited)
{
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	//esp_read_mac(baseMac, ESP_MAC_WIFI_SOFTAP);
	esp_efuse_mac_get_default(baseMac); //default is wifi sta mac
	char baseMacChr[18] = {0};

	if(delimited)
		sprintf(baseMacChr, "%02x:%02x:%02x:%02x:%02x:%02x", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	else
		sprintf(baseMacChr, "%02x%02x%02x%02x%02x%02x", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);

	return String(baseMacChr);
}

String dev_id()
{
	return get_mac_address(false).substring(6);
}

void serial_handle(){
    if(Serial.available()){
      String serial_read = Serial.readStringUntil('\r');
      String command = parse_string(serial_read,',',0);
      if(command=="s"){
        String command_type = parse_string(serial_read,',',1);
        String value = parse_string(serial_read,',',2);
        if(command_type=="mode"){
          parse_mode(value);
        } 
        else if(command_type=="server"){
          parse_server(value);
        }
        else if(command_type=="tdelay"){
          parse_timedelay(value);
          Serial.println("time update :"+ value);
        }
        else if(command_type=="time"){
          parse_time(value);
          Serial.println("time sync :" + value);
        }
        else if(command_type=="dimmer"){
          parse_dimmer(value);
          Serial.println("dimmer update :" + value);
        }
        else if(command_type=="lamppower"){
          parse_lamppower(value);
          Serial.println("lamppower" + value);
        }
      }
      else if(command=="rst"){
        ESP.restart();
        }
      //"post,wh,power,voltage,current,pf"
      else if(command == "post"){
        float volt = parse_string(serial_read,',',1).toInt();
        float curr = parse_string(serial_read,',',2).toFloat();
        float pow = parse_string(serial_read,',',3).toFloat();
        float wh = parse_string(serial_read,',',4).toFloat();
        float freq = parse_string(serial_read,',',5).toFloat();
        float pf = parse_string(serial_read,',',5).toFloat();
        Serial.println("wh:" + (String)wh + " power:" + (String)pow + " voltage:" + (String)volt + " current:" + (String)curr + " pf:" + (String)pf);
        data_process(volt,curr,pow,wh,freq,pf);
      }
    }
}

String parse_mode(const String& value){
  mode = parse_string(value,'|',0).toInt();
  if(mode == 0){
    config_t.set_mode(mode);
    ctrl = parse_string(value,'|',1).toInt();
    config_t.set_control(ctrl);
    Serial.println("mod|" + (String)mode + "|" + String(ctrl));
    change_schedule(mode);
    return "m|1"; 
  }
  else {
    config_t.set_mode(mode);
    String time_on = parse_string(value,'|',1);
    String time_off = parse_string(value,'|',2);
    config_t.set_OnSchedule(time_on, &time_on_cfg);
    on_hour      = time_on_cfg.hour; //Serial.println(on_hour);
    on_minute    = time_on_cfg.minute;
    config_t.set_OffSchedule(time_off, &time_off_cfg);
    off_hour     = time_off_cfg.hour; 
    off_minute   = time_off_cfg.minute;
    String buff = "mod|" + (String)mode + "|" + time_on + "|" + time_off;
    if(mode == 2){
      String night_time = parse_string(value,'|',3);
      config_t.set_NightSchedule(night_time, &night_time_cfg);
      night_hour     = night_time_cfg.hour; 
      night_minute   = night_time_cfg.minute;
      buff +=  "|" + night_time;
    }
    Serial.println(buff);
    change_schedule(mode);
    return "m|1";
  }
}
String parse_time(const String& value){
  //e.g: 22:20:12|26/03/2020
  String time = parse_string(value,'|',0);
  uint8_t hour = parse_string(time,':',0).toInt();
  uint8_t minute = parse_string(time,':',1).toInt();
  uint8_t second = parse_string(time,':',2).toInt();

  String datetime = parse_string(value,'|',1);
  uint8_t date = parse_string(datetime,'/',0).toInt();
  uint8_t month = parse_string(datetime,'/',1).toInt();
  uint16_t year = parse_string(datetime,'/',2).toInt();

  adjust_time(year,month,date,hour,minute,second);  
  return "timesync|1";
}
String parse_dimmer(const String& value){
  dimmer1 = parse_string(value, '|',0).toInt();
  config_t.set_dimmer1(dimmer1);
  uint8_t nightdim = parse_string(value, '|',1).toInt();
  if(nightdim != 0){
    dimmer2 = nightdim;
    config_t.set_dimmer2(dimmer2);
  }
  return "d|1";
}

String parse_server(const String& value){
  //158.140.167.173|1884|eyroMQTT|eyroMQTT1234
  backend = parse_string(value,'|',0);
  backend_port = parse_string(value,'|',1).toInt();
  backend_user = parse_string(value,'|',2);
  backend_pass = parse_string(value,'|',3);
  Serial.print("server change on backend :" 
              + config_t.set_backend(backend) 
              + " port :"
              + config_t.set_port((String)backend_port)
              + " username :"
              + config_t.set_user(backend_user)
              + " password :"
              + config_t.set_pass(backend_pass));
  return "server|1";
}

String parse_timedelay(const String& value){
  timedelay = value.toInt();
  config_t.set_timedelay(timedelay);
  return "tdelay|1";
}

String parse_lamppower(const String& value){
  lamppower = value.toInt();
  config_t.set_lamppower(lamppower);
  return "lamp|1";
}


String parse_string(String data, char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void config_all()
{
  apn     = config_t.read_apn(); Serial.println("apn :" + apn);
  backend = config_t.read_backend(); Serial.println("backend :" + backend);
  backend_port = config_t.read_port().toInt(); Serial.println("port :" + (String)backend_port);
  backend_user = config_t.read_user(); Serial.println("backend user :" + backend_user);
  backend_pass = config_t.read_pass(); Serial.println("backend pass :" + backend_pass);
  config_t.read_OnSchedule(&time_on_cfg);
  on_hour      = time_on_cfg.hour; 
  on_minute    = time_on_cfg.minute; 
  Serial.println("on :" + (String)on_hour + ":" +(String)on_minute);
  config_t.read_OffSchedule(&time_off_cfg);
  off_hour     = time_off_cfg.hour; 
  off_minute   = time_off_cfg.minute;
  Serial.println("off :" + (String)off_hour + ":" +(String)off_minute);
  config_t.read_NightSchedule(&night_time_cfg);
  night_hour     = night_time_cfg.hour; 
  night_minute   = night_time_cfg.minute;
  Serial.println("night :" + (String)night_hour + ":" +(String)night_minute);
  delay(1000);
  mode = config_t.read_mode(); Serial.println("mode :" + (String)mode);
  ctrl = config_t.read_control(); Serial.println("control :" +(String)ctrl);
  timedelay = config_t.read_timedelay(); Serial.println("tdelay :" + (String)timedelay);
  dimmer1 = config_t.read_dimmer1(); Serial.println("dimmer1 :" + (String)dimmer1);
  dimmer2 = config_t.read_dimmer2(); Serial.println("dimmer2 :" + (String)dimmer2);
  lamppower = config_t.read_lamppower(); Serial.println("lamppower :" + (String)lamppower);
}

String callback_handle(String subtopic, String payload){
    String ret_token = parse_string(payload,'-',1);
    if(ret_token.length()==TOKEN_LENGTH){
      String value = parse_string(payload,'-',0); 
      String token = "|" + ret_token;
      if       (subtopic == "/dim"){
        return parse_dimmer(value) + token;}
      else if  (subtopic == "/timesync"){
        return parse_time(value) + token;}
      else if  (subtopic == "/mode"){
        return parse_mode(value) + token;}
      // else if  (subtopic == "/cfg"){}
      else if  (subtopic =="/reset"){
        Serial.println("Restarting");
        ESP.restart();
        }
      else if (subtopic == "/resetslave"){
        Serial.println("slave reset");
        Slave.reset_mcu();
        return "rstslave|1" + token;}
      else if  (subtopic == "/rstwh"){
        Slave.reset_energy();
        return "rstwh|1" + token;}
      else if  (subtopic == "/timedelay"){
        return parse_timedelay(value) + token;}
      else if  (subtopic == "/server"){
        return parse_server(value) + token;}
      else if (subtopic == "/lamppower"){
        return parse_lamppower(value) + token;}
      else if (subtopic == "/OTA"){
        running_OTA();}
    } 
    else{
       return "";
    }
}

void slave::request_data(){

  Serial.println("#GET?sensor");
  
}

void slave::send_command(bool control){

  Serial.println("#COM?"+(String)control);

}

void slave::reset_energy(){

  Serial.println("#RSTWH");

}

void slave::reset_mcu(){
  Serial.println("#RSTMCU");
} 
