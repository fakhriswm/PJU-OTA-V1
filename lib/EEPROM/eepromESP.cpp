#include <eepromESP.h>
#include <eepromESP_table.h>

eepromESP::eepromESP()
{
	eeprom_size = 0;
	// TODO IMPLEMENTATION
	//EEPROM.begin(size);
}

////////////////////////// deprecated ////////////////////////////////
bool eepromESP::begin(size_t size)
{
	this-> eeprom_size= size;
	return EEPROM.begin(size);
}

void eepromESP::flushConfig(int start, int end){
	
	for (int i=start; i<end; ++i){
		EEPROM.write(i, 0);
	}
	EEPROM.commit();
}

void eepromESP::writeConfig(const char* input, int length, int start){
	
	for (int i=0; i<length; ++i)
	{
		EEPROM.write(start+i, input[i]); 
	}
	EEPROM.commit();
}

String eepromESP::readConfig(int start, int end){
	
	String retVal;
	for (int i=start; i<end; ++i)
	{
		if(char(EEPROM.read(i)) == 0)
			continue;
		retVal+=char(EEPROM.read(i));
	}
	return retVal;
}
//////////////////////////////// deprecated //////////////////////////////////////

void eepromESP::erase(size_t addr, size_t len)
{
	EEPROM.writeBytes(addr,0,len);
	EEPROM.commit();
}

void eepromESP::erase_all()
{
	EEPROM.writeBytes(0,"\0",eeprom_size-1);
	EEPROM.commit();
}

void eepromESP::write(size_t addr, size_t len, const void * value)
{
	erase(addr,len);
	EEPROM.writeBytes(addr,value,len);
	EEPROM.commit();
}

void eepromESP::write_byte(size_t addr, uint8_t value)
{
	EEPROM.writeByte(addr, value);
	EEPROM.commit();
}

void eepromESP::read(size_t addr,  size_t len, void * output)
{
	EEPROM.readBytes(addr, output,len);
}

uint8_t eepromESP::read_byte(size_t addr)
{
	return EEPROM.readByte(addr);
}

String eepromESP::read_str(size_t addr)
{
	return EEPROM.readString(addr);
}

void eepromESP::write_str(size_t addr, const String& str)
{
	
	size_t len=0;
	size_t read_addr = addr;
	while(EEPROM.readByte(read_addr++))
		len++;

	Serial.println("read len:" + String(len));

	erase(addr,len);
	EEPROM.writeString(addr,str);
	EEPROM.commit();
}

void eepromESP::write_str(size_t addr, const char* str)
{
	size_t len=0;
	size_t read_addr = addr;
	while(EEPROM.readByte(read_addr++))
		len++;

	erase(addr,len);
	EEPROM.writeString(addr,str);
	EEPROM.commit();
}

String eepromESP::read_ssid() 
{
    return read_str(ee_ssid);
}
void eepromESP::set_ssid(const String& ssid)
{
    write_str(ee_ssid, ssid);
}
String eepromESP::read_passwrd() 
{
    return read_str(ee_passwrd);
}
void eepromESP::set_passwrd(const String& passwrd)
{
    write_str(ee_passwrd, passwrd);
}
void eepromESP::set_onSchedule(const String& timeOn)
{
    write_str(ee_OnSchedule, timeOn);
}

String eepromESP::read_onSchedule() 
{
    return read_str(ee_OnSchedule);
}

void eepromESP::set_offSchedule(const String& timeOff)
{
    write_str(ee_OffSchedule, timeOff);
}

String eepromESP::read_offSchedule() 
{
    return read_str(ee_OffSchedule);
}

void eepromESP::set_nightSchedule(const String& nightTime)
{
    write_str(ee_NigtSchedule, nightTime);
}

String eepromESP::read_nightSchedule() 
{
    return read_str(ee_NigtSchedule);
}

void eepromESP::set_mode(uint8_t value)
{
    write_byte(ee_mode, value);
}

uint8_t eepromESP::read_mode()
{
    return read_byte(ee_mode);
}
void eepromESP::set_control(uint8_t value)
{
    write_byte(ee_control, value);
}

uint8_t eepromESP::read_control()
{
    return read_byte(ee_control);
}
void eepromESP::set_dimmer1(uint8_t value)
{
    write_byte(ee_dimmer1, value);
}

uint8_t eepromESP::read_dimmer1()
{
    return read_byte(ee_dimmer1);
}
void eepromESP::set_dimmer2(uint8_t value)
{
    write_byte(ee_dimmer2, value);
}

uint8_t eepromESP::read_dimmer2()
{
    return read_byte(ee_dimmer2);
}

void eepromESP::set_apn(const String& apn)
{
    write_str(ee_apn, apn);
}

String eepromESP::read_apn() 
{
    return read_str(ee_apn);
}

void eepromESP::set_backend(const String& backend)
{
    write_str(ee_backend, backend);
}

String eepromESP::read_backend() 
{
    return read_str(ee_backend);
}
void eepromESP::set_backend_user(const String& backend_user)
{
    write_str(ee_backend_user, backend_user);
}

String eepromESP::read_backend_user() 
{
    return read_str(ee_backend_user);
}
void eepromESP::set_backend_pass(const String& backend_pass)
{
    write_str(ee_backend_pass, backend_pass);
}

String eepromESP::read_backend_pass() 
{
    return read_str(ee_backend_pass);
}

void eepromESP::set_port(const String& back_port)
{
	 write_str(ee_port, back_port);
}
String eepromESP::read_port()
{
    return read_str(ee_port);
}
void eepromESP::set_timedelay(uint8_t value)
{
    write_byte(ee_timedelay, value);
}

uint8_t eepromESP::read_timedelay()
{
    return read_byte(ee_timedelay);
}
