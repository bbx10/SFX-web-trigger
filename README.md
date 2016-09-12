# SFX-web-trigger
Sound Effects Web Trigger

![Web User Interface](/images/screencap1.png)

Press the 0 button to play file T0.wav, press the 1 button to play file T1.wav,
etc. WAV files are larger than MP3 or OGG files but they seem to work best when
a button is pressed while a track is still playing. Interrupting MP3 or OGG
playback sometimes results in static or silence.

If the ESP8266 is unable to connect to an Access Point, it becomes an Access
Point. Connect to it by going into your wireless settings. The AP name should
look like ESP\_SFX\_TRIGGER. Once connected the ESP will bring up a web page
where you can enter the SSID and password of your WiFi router.

Load sound samples on the microSD card named T0.wav, T1.wav, ..., T9.wav. See
the samples directory for simple syntheized voice samples in all formats.

Insert the card into the VS1053 breakout board. Power up the ESP8266. Connect
any web browser to the ESP8266 web server. Connect to
http://espsfxtrigger.local. This should bring up the web page shown above.

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


## Software components

* [Adafruit VS1053 library fork](https://github.com/bbx10/Adafruit_VS1053_Library/)
has been modified to work on the ESP8266. Interrupts are not used because
calling feedBuffer() from loop() works fine. Disabling interrupts for more than 1 
msec on ESP8266 causes problem for the WiFi module. And can cause crashes.
Calling feedBuffer() from an interrupt handler is very bad on the ESP8266.

* [WiFiManager](https://github.com/tzapu/WiFiManager) eliminates
the need to store the WiFi SSID and password in the source code.  WiFiManager
will become an access point and web server to get the SSID and password if
needed.
