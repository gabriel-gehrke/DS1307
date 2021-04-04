// SEE: https://datasheets.maximintegrated.com/en/ds/DS1307.pdf

#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"

// registers
const byte SECOND = 0;
const byte MINUTE = 1;
const byte HOUR = 2;
const byte DAY = 4;
const byte MONTH = 5;
const byte YEAR = 6;

const byte LAST_4_MASK = 0b00001111;

DS1307::DS1307(int rtc_addr, int mem_addr) {
  _rtc_addr = rtc_addr;
  _mem_addr = mem_addr;
}

void DS1307::init() {
  _startOscillator();
}

void DS1307::_startOscillator() {
  setSecond(getSecond()); // ensure oscillator bit is 0 (enabled)
}

void DS1307::_write(byte addr, byte* dat, int len) {
  Wire.beginTransmission(_rtc_addr);
  Wire.write(addr);
  for (int i = 0; i < len; i++) {
    Wire.write(dat[i]);
  }
  Wire.endTransmission(true);
}

byte DS1307::_read(byte addr) {
  Wire.beginTransmission(_rtc_addr);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom(_rtc_addr, (size_t)1, true);
  return Wire.read();
}

byte DS1307::getSecond() {
  byte b = _read(SECOND);
  byte s = b & LAST_4_MASK; // ones (seconds)
  byte t = (b & 0b01110000) >> 4; // tens (seconds, first bit is ch)
  return (t * 10) + s;
}

byte DS1307::getMinute() {
  byte b = _read(MINUTE);
  byte m = b & LAST_4_MASK; // ones (minutes)
  byte t = (b & 0b01110000) >> 4; // tens (minutes, first is 0)
  return (t * 10) + m;
}

byte DS1307::getHour() {
  byte b = _read(HOUR);
  byte h = b & LAST_4_MASK; // ones (hours)
  byte t = (b & 0b00110000) >> 4; // tens (hours, first bit 0, second bit indicates 12/24 hour)
  return (t * 10) + h;
}

byte DS1307::getDay() {
  byte b = _read(DAY);
  byte d = b & LAST_4_MASK;
  byte t = (b & 0b00110000) >> 4;
  return (t * 10) + d;
}

byte DS1307::getMonth() {
  byte b = _read(MONTH);
  byte m = b & LAST_4_MASK;
  byte t = (b & 0b00010000) >> 4;
  return (t * 10) + m;
}

byte DS1307::getYear() {
  byte b = _read(YEAR);
  byte y = b & LAST_4_MASK;
  byte t = (b & 0b11110000) >> 4;
  return (t * 10) + y;
}

void DS1307::getDate(date_t* d) {
  d->second = getSecond();
  d->minute = getMinute();
  d->hour = getHour();
  d->day = getDay();
  d->month = getMonth();
  d->year = getYear();
}

void DS1307::setDate(date_t d) {
  setSecond(d.second);
  setMinute(d.minute);
  setHour(d.hour);
  setDay(d.day);
  setMonth(d.month);
  setYear(d.year);
}

static byte DS1307::_toTensFormat(byte b) {
  return ((b / 10) << 4) | (b % 10);
}

void DS1307::setSecond(byte s) {
  byte b = _toTensFormat(s);
  _write(SECOND, &b, 1);
}

void DS1307::setMinute(byte m) {
  byte b = _toTensFormat(m);
  _write(MINUTE, &b, 1);
}

void DS1307::setHour(byte h) {
  byte b = _toTensFormat(h);
  _write(HOUR, &b, 1);
}

void DS1307::setDay(byte d) {
  byte b = _toTensFormat(d);
  _write(DAY, &b, 1);
}

void DS1307::setMonth(byte m) {
  byte b = _toTensFormat(m);
  _write(MONTH, &b, 1);
}

void DS1307::setYear(byte y) {
  byte b = _toTensFormat(y);
  _write(YEAR, &b, 1);
}
