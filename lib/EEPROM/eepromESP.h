#include <Arduino.h>
#include <EEPROM.h>

class eepromESP {
    
    public :
        eepromESP();
		bool begin(size_t size);
		void flushConfig(int start, int end);
		void  writeConfig(const char* input, int length, int start);
		String readConfig(int start, int end);

		void erase(size_t addr, size_t len);
		void erase_all();

		void write_byte(size_t addr, uint8_t value);
		void write(size_t addr, size_t len, const void * value);
		void write_str(size_t addr, const String& str);
		void write_str(size_t addr, const char* str);

		void read(size_t addr,  size_t len, void * output);
		uint8_t read_byte(size_t addr);
		String read_str(size_t addr);   
        size_t eeprom_size; 
        void set_onSchedule (const String& timeOn);
        String read_onSchedule ();
        void set_offSchedule (const String& timeOff);
        String read_offSchedule ();
        void set_nightSchedule (const String& nightTime);
        String read_nightSchedule ();
        void set_mode(uint8_t value);
        uint8_t read_mode();
        void set_control(uint8_t value);
        uint8_t read_control();
        void set_dimmer1(uint8_t value);
        uint8_t read_dimmer1();
        void set_dimmer2(uint8_t value);
        uint8_t read_dimmer2();
        void set_apn(const String& apn);
        String read_apn();
        void set_backend(const String& backend);
        String read_backend();
        void set_backend_user(const String& backend_user);
        String read_backend_user();
        void set_backend_pass(const String& backend_pass);
        String read_backend_pass();
        void set_port(const String& back_port);
        String read_port();
        void set_timedelay(uint8_t value);
        uint8_t read_timedelay();
        void set_lamppower(uint16_t lamppower);
        uint16_t read_lamppower();
        void set_ssid(const String& ssid);
        String read_ssid();
        void set_pwssid(const String& pwssid);
        String read_pwssid();
};
