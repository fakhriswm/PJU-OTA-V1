/*
 * tool.h
 *
 *  Created on: Aug 7, 2019
 *      Author: luqma
 */

#ifndef LIB_TOOL_TOOL_H_
#define LIB_TOOL_TOOL_H_

#include "Arduino.h"

extern String apn;
extern String backend;
extern String backend_user;
extern String backend_pass;
extern uint16_t backend_port;
extern uint32_t on_hour; 
extern uint32_t on_minute; 
extern uint32_t on_second; 
extern uint32_t off_hour; 
extern uint32_t off_minute; 
extern uint32_t off_second;
extern uint32_t night_hour; 
extern uint32_t night_minute;
extern uint8_t mode;
extern uint8_t ctrl;
extern uint16_t timedelay;
extern uint8_t dimmer1;
extern uint8_t dimmer2;
extern uint16_t lamppower;
extern String ota_server;
extern String ota_resource;
extern uint16_t ota_port;
extern bool command_lamp;
extern bool task_update;

extern void data_process(float volt, float curr, float pow, float wh, float freq, float pf);
extern void adjust_time(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second);
extern void change_schedule(uint8_t change_mod);
extern void ceate_OTAtask();

void serial_handle();
void config_all();
String get_mac_address(bool delimited=true);
String dev_id();
String parse_string(String data, char separator, int index);
String parse_mode(const String& value);
String parse_time(const String& value);
String parse_server(const String& value);
String parse_dimmer(const String& value);
String parse_OTA(const String& value);
String parse_timedelay(const String& value);
String parse_lamppower(const String& value);
String callback_handle(String subtopic, String payload);

class slave {
    public : 
    void request_data();
    void send_command(bool control);
    void reset_energy();
    void reset_mcu();
};

#endif /* LIB_TOOL_TOOL_H_ */
