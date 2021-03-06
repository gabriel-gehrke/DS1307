#include <Arduino.h>

#ifndef DS1307_h
#define DS1307_h

const uint32_t SECONDS_IN_MINUTE = 60;
const uint32_t SECONDS_IN_HOUR = SECONDS_IN_MINUTE * 60;
const uint32_t SECONDS_IN_DAY = SECONDS_IN_HOUR * 24;

enum Weekday : byte {
  Monday = 0,
  Tuesday = 1,
  Wednesday = 2,
  Thursday = 3,
  Friday = 4,
  Saturday = 5,
  Sunday = 6
};

// DATE STRUCT
typedef struct Date
{
  public:
    byte year;
    byte month;
    byte day;
    byte hour;
    byte minute;
    byte second;
    
  
    Weekday getWeekday() {
      return (toDays() + Saturday) % 7; // 01.01.2001 was a Saturday
    } 

    uint32_t toDays() {
      
      uint32_t d = day - 1;
      
      for (byte i = 0; i < year; i++) {
        d += 365 + _isLeapYear(i); // 365 in non-leap-year
      }
      for (byte i = 1; i < month; i++) {
        if (i <= 7) {
          d += 30 + (i%2); // months 1, 3, 5, 7 have 31 days
        }
        else {
          d += 31 - (i%2); // months 8, 10, 12 have 31 days
        }
      }
      if (month >= 3) {
        d -= (2 - _isLeapYear(year)); // remove 1 day in leap years, 2 in normal years
      }
      return d;
    }

    uint32_t toSeconds() {
      return second + (toDays() * SECONDS_IN_DAY) + (minute * SECONDS_IN_MINUTE) + (hour * SECONDS_IN_HOUR);
    }

    char* toString(char* buff) {
      sprintf(buff, "%02d:%02d:%02d-%02d.%02d.%02d", hour, minute, second, day, month, year);
      return buff;
    }

  private:
    static bool _isLeapYear(byte y) {
      return (y%4) == 0;
    }
} date_t;

class DS1307
{
  public:
    DS1307(int rtc_addr, int mem_addr);
    void init();
    byte getSecond();
    byte getMinute();
    byte getHour();
    byte getDay();
    byte getMonth();
    byte getYear();
    void setSecond(byte s);
    void setMinute(byte m);
    void setHour(byte h);
    void setDay(byte d);
    void setMonth(byte m);
    void setYear(byte y);
    void getDate(date_t* d);
    void setDate(date_t* d);

    void writeToRam(byte addr, void* data, size_t data_size);
    void readFromRam(byte addr, void* data, size_t data_size);
  private:
    int _rtc_addr;
    int _mem_addr;
    void _startOscillator();
    byte _read(byte addr);
    void _readSequential(byte addr, byte* buff, size_t len);
    void _write(byte addr, byte dat);
    void _writeSequential(byte addr, byte* dat, size_t len);
    static byte _toTensFormat(byte b);
    static byte _fromTensFormat(byte b, byte mask);
};

#endif
