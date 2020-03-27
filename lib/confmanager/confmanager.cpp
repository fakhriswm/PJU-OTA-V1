#include <confmanager.h>
#include <eepromESP.h>

eepromESP conf;

void confmanager::set_OnSchedule (const String& payload, time_on_config_t* time_on_cfg){
    conf.set_onSchedule(payload);
    delay(10);
    read_OnSchedule(time_on_cfg);
}

void confmanager::set_OffSchedule (const String& payload, time_off_config_t* time_off_cfg){
    conf.set_offSchedule(payload);
    delay(10);
    read_OffSchedule(time_off_cfg);
}

void confmanager::set_NightSchedule (const String& payload, night_time_config_t* night_time_cfg){
    conf.set_nightSchedule(payload);
    delay(10);
    read_NightSchedule(night_time_cfg);
}
uint8_t confmanager::set_control (uint8_t payload){
    if(payload==0 || payload==1){
        conf.set_control(payload);
        delay(10);
        return payload;
  }
  else {
      return 0;
  }
}
uint8_t confmanager::set_mode (uint8_t payload){
  if(payload>=0 && payload<=2){
        conf.set_mode(payload);
        delay(10);
        return payload;
  }
  else {
      return 0;
  }
}
uint8_t confmanager::set_timedelay(uint8_t payload){
    if(payload>0){
        conf.set_timedelay(payload);
        return payload;
    }
    else{
        return 0;
    }
} 
String confmanager::set_port (const String& payload){
    conf.set_port(payload);
    delay(10);
    return read_port();
}
uint8_t confmanager::read_control(){
    uint8_t value = conf.read_control();
    return value;
}

uint8_t confmanager::read_mode(){
    uint8_t value = conf.read_mode();
    return value;
}

uint8_t confmanager::read_timedelay(){
    uint8_t value = conf.read_timedelay();
    return value;
}

String confmanager::read_port(){
    String value = conf.read_port();
    return value;
}

void confmanager::read_OnSchedule(time_on_config_t* time_on_cfg){
    time_on_cfg-> hour = conf.read_onSchedule().substring(0,2).toInt();
    time_on_cfg-> minute = conf.read_onSchedule().substring(3,5).toInt();
}

void confmanager::read_OffSchedule(time_off_config_t* time_off_cfg){
    time_off_cfg-> hour = conf.read_offSchedule().substring(0,2).toInt();
    time_off_cfg-> minute = conf.read_offSchedule().substring(3,5).toInt();
} 

void confmanager::read_NightSchedule(night_time_config_t* night_time_cfg){
    night_time_cfg-> hour = conf.read_nightSchedule().substring(0,2).toInt();
    night_time_cfg-> minute = conf.read_nightSchedule().substring(3,5).toInt();
}

String confmanager::set_backend(const String& payload){
    conf.set_backend(payload);
    return read_backend();
}
String confmanager::read_backend(){
    String value = conf.read_backend();
    return value;
}

String confmanager::set_user(const String& payload){
    conf.set_backend_user(payload);
    return read_user();
}
String confmanager::read_user(){
    String value = conf.read_backend_user();
    return value;
}
String confmanager::set_pass(const String& payload){
    conf.set_backend_pass(payload);
    return read_pass();
}
String confmanager::read_pass(){
    String value = conf.read_backend_pass();
    return value;
}
String confmanager::set_apn(const String& payload){
    conf.set_apn(payload);
    return read_apn();
}
String confmanager::read_apn(){
    String value = conf.read_apn();
    return value;
}

uint8_t confmanager::set_dimmer1(uint8_t payload){
    if(payload>0){
        conf.set_dimmer1(payload);
        return payload;
    }
    else{
        return 0;
    }
}

uint8_t confmanager::set_dimmer2(uint8_t payload){
    if(payload>0){
        conf.set_dimmer2(payload);
        return payload;
    }
    else{
        return 0;
    }
}

uint8_t confmanager::read_dimmer1(){
    uint8_t value = conf.read_dimmer1();
    return value;
}

uint8_t confmanager::read_dimmer2(){
    uint8_t value = conf.read_dimmer2();
    return value;
}


