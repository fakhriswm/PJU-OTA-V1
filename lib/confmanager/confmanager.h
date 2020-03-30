#include <Arduino.h>


typedef struct 
{
    uint16_t minute;
    uint16_t hour;
}time_on_config_t;

typedef struct{   
    uint16_t minute;
    uint16_t hour;
}time_off_config_t;

typedef struct{   
    uint16_t minute;
    uint16_t hour;
}night_time_config_t;

class confmanager {

public :
    
    
    void set_OnSchedule (const String& payload, time_on_config_t* time_on_cfg);
    void set_OffSchedule (const String& payload, time_off_config_t* time_off_cfg);
    void set_NightSchedule (const String& payload, night_time_config_t* night_time_cfg);
    uint8_t set_control (uint8_t payload);
    uint8_t set_mode (uint8_t payload);
    uint8_t set_timedelay (uint8_t payload);
    uint16_t set_lamppower(uint16_t payload);
    String set_port (const String& payload);
    String set_backend(const String& payload);
    String set_user(const String& payload);
    String set_pass(const String& payload);
    String set_apn(const String& payload);
    uint8_t set_dimmer1 (uint8_t payload);
    uint8_t set_dimmer2 (uint8_t payload);
    uint8_t read_control();
    uint8_t read_mode();
    uint8_t read_timedelay();
    uint8_t read_dimmer1();
    uint8_t read_dimmer2();
    uint16_t read_lamppower();
    String read_port();
    String read_backend();
    String read_user();
    String read_pass();
    String read_apn();
    void read_OnSchedule(time_on_config_t* time_on_cfg);
    void read_OffSchedule(time_off_config_t* time_off_cfg);
    void read_NightSchedule(night_time_config_t* night_time_cfg);
};