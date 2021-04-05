# DS1307 Real Time Clock (RTC) Arduino Library
A lightweight library driving the 1307 rtc chip, written in C++ for projects in the Arduino IDE.<br>
Since the DS1307 is a i2c device, the library uses the ``Wire.h`` library.<br>

### Getting Started
To get started, you need to import both the ``Wire.h`` and the ``DS1307.h`` header files.
``` C++
#include <Wire.h>
#include <DS1307.h>
```
Then, to communicate with the DS1307 board, you first need to declare an instance of the DS1307 class. The constructor takes two address bytes as parameters. The first byte is the i2c address of the DS1307 chip itself, which (according to the documentation I could find) is 0x68.
The second address leads to the EEPROM (AT24C32) (CURRENTLY UNSUPPORTED) which is often part of shipped boards and also directly accessible from the i2c bus.
``` C++
DS1307 rtc(0x68, 0x50);
```
In the ``setup`` method, you then need to initialize the i2c bus and call the rtc's ``init`` method.
``` C++
void setup() {
    Wire.begin(); // initialize i2c bus
    rtc.init(); // initialize rtc
}
```

## Date Type
The ``date_t`` struct/type is the main datastructure you would want to work with. You can pass ``date_t*`` (pointers to date_t instances)
to the rtc's ``getDate()`` and ``setDate()`` methods to capture all data coming from the rtc or to set the time and date in one flow.<br>
#### Variables contained in ``date_t`` type:
* ``year`` (0-99)
* ``month`` (1-12)
* ``day`` (1-31)
* ``hour`` (0-23)
* ``minute`` (0-59)
* ``second`` (0-59)

All values are stored as bytes, which are unsigned 8-bit integers in the Arduino framework.<br>
Therefore, a ``date_t`` instance will only take up 6 bytes of memory.

### Reading Time and Date
``` C++
byte second = rtc.getSecond();
byte minute = rtc.getMinute();
byte hour = rtc.getHour();
byte day = rtc.getDay();
byte month = rtc.getMonth();
byte year = rtc.getYear();
```
Or, alternatively:
``` C++
date_t d; // date struct
rtc.getDate(&d); // updates values of the date struct pointed towards

byte s = d.second;
byte m = d.minute;
byte h = d.hour;
// ...
```

### Setting Time and Date
``` C++
rtc.setYear(21); // 2021
rtc.setMonth(4); // April
rtc.setDay(3); // 3rd

rtc.setHour(15); // 3pm, 24-hour format
rtc.setMinute(30); // 30 minutes
rtc.setSecond(27); // 27 seconds
```
Or, alternatively:
``` C++
date_t newdate = {21, 4, 3, 15, 30, 27};
rtc.setDate(&newdate);
```

### Conversion Methods
The ``date_t`` type also defines some handy methods for working with time and dates.
``` C++
date_t d;
rtc.getDate(&d);

uint32_t seconds = d.toSeconds() // seconds passed since millenium (year 00 at 00:00:00)
uint32_t days = d.toDays() // days since millenium (year 00 at 00:00:00)
```
Allowing calculations like:
``` C++
int daysBetweenDates(date_t* d1, date_t* d2) {
    // casting to signed integer type, because toDays() returns unsigned integer type
    return (int)d2->toDays() - (int)d1->toDays();
}
```

## Additional Features
The DS1307 has 64 bytes of RAM, while only 8 bytes of them are actually used.
Therefore, you can store up to 56 bytes of data on the DS1307's RAM.<br>
Because the DS1307 is usually powered by the backup battery attached to it's board, the RAM won't erase after the Arduino's
shutdown. This way you can save data between power cycles.

#### Save Data to RAM
```C++
char* data = "This is a test"; // 15 bytes of data (14 chars + null-termination)
rtc.writeToRam(
    0, // address offset (0-57)
    data, // pointer to data (as void* is expected, you can pass any pointer)
    15 // number of bytes to write (1-58)
);
```

#### Read Data from RAM
```C++
char buffer[100]; // buffer used to store the read data
rtc.readFromRam(
    0, // address offset (0-57)
    buffer, // pointer to buffer (as void* is expected, you can pass any pointer)
    15 // number of bytes to read (1-58)
);
Serial.println(buffer);
// Output: "This is a test"
```

## Future Plans
In the future, this library might support:
* Timer/Alarm setup
* The AT24C32 EEPROM
* Further methods for working with dates
