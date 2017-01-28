/*****************************************************************************
  The MIT License (MIT)

  Copyright (c) 2016-2017 by bbx10node@gmail.com

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 **************************************************************************/

/*
 * ESP8266 MP3, WAV, OGG, etc. Sound Effects (SFX) Web Trigger
 */


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets
#include <Hash.h>

/*
 * VS1053 chip converts MP3, WAV, OGG, etc. to analog audio output
 * This board also has micro SD card slot.
 */
#include <SPI.h>
#include <Adafruit_VS1053.h>    // https://github.com/adafruit/Adafruit_VS1053_Library
#include <SD.h>

//#define FEATHERWING
#ifdef FEATHERWING
// These are the pins used with Feather ESP8266 and VS1053 FeatherWing
#define BREAKOUT_RESET  -1  // VS1053 reset pin (output)
#define BREAKOUT_CS     16  // VS1053 chip select pin (output)
#define BREAKOUT_DCS    15  // VS1053 Data/command select pin (output)
#define CARDCS          2   // Card chip select pin
#define DREQ            0   // VS1053 Data request
#else
// These are the pins used with ESP8266 and Adafruit VS1053 breakout
#define BREAKOUT_RESET  0   // VS1053 reset pin (output)
#define BREAKOUT_CS     5   // VS1053 chip select pin (output)
#define BREAKOUT_DCS    16  // VS1053 Data/command select pin (output)
#define CARDCS          15  // Card chip select pin
#define DREQ            4   // VS1053 Data request
#endif

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

void mp3_setup(void)
{
  while (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    delay(500);
  }
  Serial.println(F("SD OK!"));

  Serial.printf("Adafruit_VS1053_FilePlayer(%d, %d, %d, %d, %d)\n", BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    delay(1000);
    ESP.restart();
  }
  Serial.println(F("VS1053 found"));

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  // Interrupts not used on ESP8266
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
}

inline void mp3_loop(void) {
  // This feeds audio data to the VS1053.
  musicPlayer.feedBuffer();
}

/*************************************************************************************/

/*
 * index.html
 */
// This string holds HTML, CSS, and Javascript for the HTML5 UI.
// The browser must support HTML5 WebSockets which is true for all modern browsers.
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>ESP8266 Sound Effects Web Trigger</title>
<style>
body { font-family: Arial, Helvetica, Sans-Serif; }
table { border: 1px solid black; }
th { border: 1px solid black; }
td { text-align: left; border: 1px solid black; }
.SFXtd { text-align: right; }
.SFXButton { font-size: 125%; background-color: #E0E0E0; border: 1px solid; }
</style>
<script>
function enableTouch(objname) {
  console.log('enableTouch', objname);
  var e = document.getElementById(objname);
  if (e) {
    e.addEventListener('touchstart', function(event) {
        event.preventDefault();
        console.log('touchstart', event);
        buttondown(e);
        }, false );
    e.addEventListener('touchend',   function(event) {
        console.log('touchend', event);
        buttonup(e);
        }, false );
  }
  else {
    console.log(objname, ' not found');
  }
}

var websock;
var WebSockOpen=0;  //0=close,1=opening,2=open

function start() {
  websock = new WebSocket('ws://' + window.location.hostname + ':81/');
  WebSockOpen=1;
  websock.onopen = function(evt) {
    console.log('websock open');
    WebSockOpen=2;
    var e = document.getElementById('webSockStatus');
    e.style.backgroundColor = 'green';
  };
  websock.onclose = function(evt) {
    console.log('websock close');
    WebSockOpen=0;
    var e = document.getElementById('webSockStatus');
    e.style.backgroundColor = 'red';
  };
  websock.onerror = function(evt) { console.log(evt); };
  websock.onmessage = function(evt) {
    var nowPlaying = document.getElementById('nowPlaying');
    if (evt.data.startsWith('nowPlaying=')) {
      nowPlaying.innerHTML = evt.data;
    }
    else {
      console.log('unknown event', evt.data);
    }
  };

  var allButtons = [
    'bSFX0',
    'bSFX1',
    'bSFX2',
    'bSFX3',
    'bSFX4',
    'bSFX5',
    'bSFX6',
    'bSFX7',
    'bSFX8',
    'bSFX9',
  ];
  for (var i = 0; i < allButtons.length; i++) {
    enableTouch(allButtons[i]);
  }
}

function buttondown(e) {
  switch (WebSockOpen) {
    case 0:
      window.location.reload();
      WebSockOpen=1;
      break;
    case 1:
    default:
      break;
    case 2:
      websock.send(e.id + '=1');
      break;
  }
}

function buttonup(e) {
  switch (WebSockOpen) {
    case 0:
      window.location.reload();
      WebSockOpen=1;
      break;
    case 1:
    default:
      break;
    case 2:
      websock.send(e.id + '=0');
      break;
  }
}
</script>
</head>
<body onload="javascript:start();">
<h2>ESP SF/X Web Trigger</h2>
<div id="nowPlaying">Now Playing</div>
<table>
  <tr>
    <td class="SFXtd"><button id="bSFX0" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">0</button></td>
    <td>Track 0</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX1" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">1</button></td>
    <td>Track 1</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX2" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">2</button></td>
    <td>Track 2</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX3" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">3</button></td>
    <td>Track 3</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX4" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">4</button></td>
    <td>Track 4</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX5" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">5</button></td>
    <td>Track 5</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX6" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">6</button></td>
    <td>Track 6</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX7" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">7</button></td>
    <td>Track 7</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX8" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">8</button></td>
    <td>Track 8</td>
  </tr>
  <tr>
    <td class="SFXtd"><button id="bSFX9" type="button" class="SFXButton"
      onmousedown="buttondown(this);" onmouseup="buttonup(this);">9</button></td>
    <td>Track 9</td>
  </tr>
</table>
<p>
</body>
</html>
)rawliteral";

/*
 * Web server and websocket server
 */
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
bool Playing;

void startPlaying(const char *filename)
{
  char nowPlaying[80] = "nowPlaying=";

  musicPlayer.stopPlaying();
  delay(2);
  musicPlayer.startPlayingFile(filename);
  strncat(nowPlaying, filename, sizeof(nowPlaying)-strlen(nowPlaying)-1);
  webSocket.broadcastTXT(nowPlaying);
  Playing = true;
}

void update_browser() {
  if (Playing && musicPlayer.stopped()) {
    Playing = false;
    webSocket.broadcastTXT("nowPlaying=");
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);

      // Looks for button press "bSFXn=1" messages where n='0'..'9'
      if ((length == 7) &&
          (memcmp((const char *)payload, "bSFX", 4) == 0) &&
          (payload[6] == '1')) {
        switch (payload[4]) {
          case '0':
            startPlaying("T0.mp3");
            break;
          case '1':
            startPlaying("T1.mp3");
            break;
          case '2':
            startPlaying("T2.mp3");
            break;
          case '3':
            startPlaying("T3.mp3");
            break;
          case '4':
            startPlaying("T4.mp3");
            break;
          case '5':
            startPlaying("T5.mp3");
            break;
          case '6':
            startPlaying("T6.mp3");
            break;
          case '7':
            startPlaying("T7.mp3");
            break;
          case '8':
            startPlaying("T8.mp3");
            break;
          case '9':
            startPlaying("T9.mp3");
            break;
        }
      }
      else {
        Serial.printf("Unknown message from client [%s]\r\n", payload);
      }

      // send message to client
      // webSocket.sendTXT(num, "message here");

      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      hexdump(payload, length);

      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}

void webserver_setup(void)
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("websocket server started");

  if(MDNS.begin("espsfxtrigger")) {
    Serial.println(F("MDNS responder started. Connect to http://espsfxtrigger.local/"));
  }
  else {
    Serial.println(F("MDNS responder failed"));
  }

  // handle "/"
  server.on("/", []() {
      server.send_P(200, "text/html", INDEX_HTML);
  });

  server.begin();
  Serial.println("web server started");

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

inline void webserver_loop(void) {
  webSocket.loop();
  server.handleClient();
  update_browser();
}

/**************************************************************************************/

/*
 * WiFiManager (https://github.com/tzapu/WiFiManager)
 */
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

void wifiman_setup(void)
{
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset saved settings. Clears SSID and password
  //wifiManager.resetSettings();

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("ESP_SFX_Trigger");

  //if you get here you have connected to the WiFi
  Serial.println(F("connected"));
}

/*
 * Arduino setup()
 */
void setup()
{
  Serial.begin(115200);
  Serial.println(F("\nESP8266 Sound Effects Web Trigger"));

  wifiman_setup();
  mp3_setup();
  webserver_setup();
}

/*
 * Arduino loop()
 */
void loop(void)
{
  mp3_loop();
  webserver_loop();
}
