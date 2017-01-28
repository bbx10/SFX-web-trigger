# SFX-web-trigger
Sound Effects Web Trigger

![Web User Interface](/images/screencap1.png)

Press the 0 button to play file T0.mp3, press the 1 button to play file T1.mp3,
etc. WAV, MP3, and OGG work fine when interrupted. Previously, interrupted
playback of MP3 sometimes resulted in static or silence.

If the ESP8266 is unable to connect to an Access Point, it becomes an Access
Point. Connect to it by going into your wireless settings. The AP name should
look like ESP\_SFX\_TRIGGER. Once connected the ESP will bring up a web page
where you can enter the SSID and password of your WiFi router.

Load sound samples on the microSD card named T0.mp3, T1.mp3, ..., T9.mp3. See
the samples directory for simple synthesized voice samples in all formats.

Insert the card into the VS1053 breakout board. Power up the ESP8266. Connect
any web browser to the ESP8266 web server. Connect to
http://espsfxtrigger.local. This should bring up the web page shown above.
If the URL does not, work connect to the ESP IP address.

You could add a battery, amplifier, and small speaker to the ESP8266 and VS1053
breakout. Load up your favorite sound samples such as elephant trumpet, minion
laugh, cat meow, sad trombone, etc. Bury the device in the couch cushions.
Wait for your victim to sit down. While you appear to play Candy Crush on your
phone, remotely trigger your favorite sound F/X!

## Hardware components

* [Adafruit VS1053 Codec + MicroSD Breakout](https://www.adafruit.com/products/1381)
This project uses the VS1053 and the MicroSD slot. However, it could be modified to
store the sound effects files in the ESP8266 SPIFFS Flash file system. The
VS1053 converts WAV, MP3, OGG, and other audio file formats to analog stereo
output.

* [Adafruit Huzzah ESP8266 Breakout](https://www.adafruit.com/products/2471)
Any similar ESP8266 board should work such as Wemos D1 Mini or NodeMCU
Devkit 1.0. If you want battery power, the Adafruit Feather Huzzah is a good
choice.

* Jumper wires, headphones, earbuds, etc.

## Connection Diagram

### ESP8266 Huzzah and VS1053 Breakout board

Adafruit VS1053 BoB |ESP8266 |Headphone Jack |Description
--------------------|--------|---------------|----------------------
VCC                 |5V      |               |
GND                 |GND     |               |
CLK                 |#14     |               |SPI clock
MISO                |#12     |               |SPI MISO
MOSI                |#13     |               |SPI MOSI
CS                  |#5      |               |SPI CS VS1053
RST                 |#0      |               |Reset
XDCS                |#16     |               |Data/Command select
SDCS                |#15     |               |SPI CS uSD card slot
DREQ                |#4 (IRQ)|               |VS1053 interrupt
                    |        |               |
AGND                |        |   Center      |
LOUT                |        |   Left        |Left audio channel
ROUT                |        |   Right       |Right audio channel

### Feather ESP8266 and VS1053 FeatherWing

Just plug the FeatherWing in the Feather. Uncomment `#define FEATHER` in the
sketch.

* [Adafruit Feather HUZZAH with ESP8266 WiFi](https://www.adafruit.com/products/2821)
* [Adafruit Music Maker FeatherWing](https://www.adafruit.com/products/3357)


## Software components

Testing was done using Arduino IDE 1.6.13. ESP8266 board support was installed
from github.com. This version has many fixes since the last stable release.

https://github.com/esp8266/arduino#using-git-version

All of the following libraries may be installed using the Arduino IDE Library
Manager. The forked VS1053 library should no longer be used. The Adafruit
version works fine.

* [Adafruit VS1053 library](https://github.com/adafruit/Adafruit_VS1053_Library/)
has been modified to work on the ESP8266. This sketch does not use interrupts
because calling feedBuffer() from loop() works fine. Disabling interrupts for
more than 1 msec on ESP8266 causes problem for the WiFi module. And can cause
crashes. Calling feedBuffer() from an interrupt handler is very bad on the
ESP8266.

* [WiFiManager](https://github.com/tzapu/WiFiManager) eliminates the need to
store the WiFi SSID and password in the source code. WiFiManager switches to
access point mode and presents a web server to get the SSID and password, when
needed.

* [WebSockets](https://github.com/Links2004/arduinoWebSockets) can be installed
using the IDE library manager. WebSockets is used to update the now playing
field on the web page. And to send HTML button press events to the ESP8266
web server.

## Changes

* Add support for ESP8266 Feather and VS1053 FeatherWing.
* Use latest Adafruit VS1053 library instead of my fork.
* Use ESP8266 board support package from github.
