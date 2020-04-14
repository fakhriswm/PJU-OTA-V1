#include <timeGSM.h>

void timeGSM ::TimeInit()
{
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
     }
}

String timeGSM :: get_completeTime(){
    char buf[25];
    DateTime datetime = rtc.now();
    sprintf(buf,"%02d:%02d:%02d %02d/%02d/%02d", datetime.hour(),datetime.minute(),datetime.second(),datetime.day(),datetime.month(),datetime.year());
    Serial.println(buf);
    return buf;
}

void timeGSM :: getGSMtime(String GSMtime)
{
    //19/12/26,10:57:35+28
    delay(1000);
    GSMyear = GSMtime.substring(0,2).toInt();
    GSMmonth = GSMtime.substring(3,5).toInt();
    GSMdate = GSMtime.substring(6,8).toInt();
    GSMhour = GSMtime.substring(9,11).toInt();
    GSMminute = GSMtime.substring(12,14).toInt();
    GSMsecond = GSMtime.substring(15,17).toInt();
    //Serial.print(GSMdate); Serial.print("/"); Serial.print(GSMmonth); Serial.print("/"); Serial.print(GSMyear);Serial.print(",");
    Serial.print(GSMhour); Serial.print(":"); Serial.print(GSMminute); Serial.print(":"); Serial.println(GSMsecond);
    adjust_rtc(GSMyear, GSMmonth, GSMdate, GSMhour, GSMminute, GSMsecond);
}

void timeGSM :: adjust_rtc(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute,  uint8_t second)
{
     rtc.adjust(DateTime(year, month, date, hour, minute, second));
}

uint8_t timeGSM :: getSecond()
{
    DateTime now = rtc.now();
    return now.second();
}

uint8_t timeGSM :: getMinute()
{
    DateTime now = rtc.now();
    return now.minute();
}

uint8_t timeGSM :: getHour()
{
    DateTime now = rtc.now();
    return now.hour();
}

uint8_t timeGSM :: getDate()
{
    DateTime now = rtc.now();
    return now.day();
}

uint8_t timeGSM :: getMonth()
{
    DateTime now = rtc.now();
    return now.month();
}

uint16_t timeGSM :: getYear()
{
    DateTime now = rtc.now();
    return now.year();
};
