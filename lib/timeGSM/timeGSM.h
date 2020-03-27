#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>

class timeGSM
{
    private :
        RTC_DS3231 rtc;
    public :
        void getGSMtime(String GSMtime);
        void adjust_rtc(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second);
        void TimeInit();
        uint8_t getSecond();
        uint8_t getMinute();
        uint8_t getHour();
        uint8_t getDate();
        uint8_t getMonth();
        uint16_t getYear();
        uint16_t GSMyear;
        uint8_t GSMmonth;
        uint8_t GSMdate;
        uint8_t GSMhour;
        uint8_t GSMminute;
        uint8_t GSMsecond;
};