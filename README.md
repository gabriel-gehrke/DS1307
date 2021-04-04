# DS1307 Real Time Clock (RTC)
A lightweight library driving the 1307 rtc chip, written in C++ for projects in the Arduino IDE.
Since the DS1307 is a i2c device, the library uses the ``Wire.h`` library.<br>

To get started, you need to import both the ``Wire.h`` and the ``DS1307.h`` header file.
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
