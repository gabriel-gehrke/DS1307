// SEE: https://datasheets.maximintegrated.com/en/ds/DS1307.pdf

#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"

// registers
const byte SECOND_REG = 0;
const byte MINUTE_REG = 1;
const byte HOUR_REG = 2;
const byte WEEKDAY_REG = 3; // unused for now
const byte DAY_REG = 4;
const byte MONTH_REG = 5;
const byte YEAR_REG = 6;

const byte RAM_REG = 8;

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

/* I2C COMMUNICATION */

void DS1307::_write(byte addr, byte dat) {
  _writeSequential(addr, &dat, 1);
}

void DS1307::_writeSequential(byte addr, byte* dat, size_t len) {
  Wire.beginTransmission(_rtc_addr);
  Wire.write(addr);
  for (size_t i = 0; i < len; i++) {
    Wire.write(dat[i]);
  }
  Wire.endTransmission(true);
}

byte DS1307::_read(byte addr) {
  byte b;
  _readSequential(addr, &b, 1);
  return b;
}

void DS1307::_readSequential(byte addr, byte* buff, size_t len) {
  Wire.beginTransmission(_rtc_addr);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom(_rtc_addr, len, true);
  for (size_t i = 0; i < len; i++) {
    buff[i] = Wire.read();
  }
}

/* ---------------------- */

byte DS1307::getSecond() {
  return _fromTensFormat(_read(SECOND_REG), 0b01110000);
}

byte DS1307::getMinute() {
  return _fromTensFormat(_read(MINUTE_REG), 0b01110000);
}

byte DS1307::getHour() {
  return _fromTensFormat(_read(HOUR_REG), 0b00110000);
}

byte DS1307::getDay() {
  return _fromTensFormat(_read(DAY_REG), 0b00110000);
}

byte DS1307::getMonth() {
  return _fromTensFormat(_read(MONTH_REG), 0b00010000);
}

byte DS1307::getYear() {
  return _fromTensFormat(_read(YEAR_REG), 0b11110000);
}

static byte DS1307::_fromTensFormat(byte b, byte mask) {
  byte singles = b & 0b00001111;
  byte tens = (b & mask) >> 4;
  return (tens * 10) + singles;
}

void DS1307::getDate(date_t* d) {
  d->second = getSecond();
  d->minute = getMinute();
  d->hour = getHour();
  d->day = getDay();
  d->month = getMonth();
  d->year = getYear();
}


// WRITE CONFIG

void DS1307::setDate(date_t* d) {
  const size_t SET_WHOLE_LEN = 7;
  byte data[SET_WHOLE_LEN];
  
  data[SECOND_REG] = _toTensFormat(d->second);
  data[MINUTE_REG] = _toTensFormat(d->minute);
  data[HOUR_REG] = _toTensFormat(d->hour);
  data[WEEKDAY_REG] = 0x00;
  data[DAY_REG] = _toTensFormat(d->day);
  data[MONTH_REG] = _toTensFormat(d->month);
  data[YEAR_REG] = _toTensFormat(d->year);

  _writeSequential(SECOND_REG, data, SET_WHOLE_LEN);
}

static byte DS1307::_toTensFormat(byte b) {
  return ((b / 10) << 4) | (b % 10);
}

void DS1307::setSecond(byte s) {
  _write(SECOND_REG, _toTensFormat(s));
}

void DS1307::setMinute(byte m) {
  _write(MINUTE_REG, _toTensFormat(m));
}

void DS1307::setHour(byte h) {
  _write(HOUR_REG, _toTensFormat(h));
}

void DS1307::setDay(byte d) {
  _write(DAY_REG, _toTensFormat(d));
}

void DS1307::setMonth(byte m) {
  _write(MONTH_REG, _toTensFormat(m));
}

void DS1307::setYear(byte y) {
  _write(YEAR_REG, _toTensFormat(y));
}

// ACESS RAM

void DS1307::writeToRam(byte addr, void* data, size_t data_size) {
  _writeSequential(RAM_REG + addr, (byte*)data, data_size);
}

void DS1307::readFromRam(byte addr, void* buffer, size_t data_size) {
  _readSequential(RAM_REG + addr, (byte*)buffer, data_size);
}
