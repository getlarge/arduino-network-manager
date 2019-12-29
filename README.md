# Network Manager - Arduino

ESP8266 | ESP32 Arduino library to handle WiFi - MQTT - HTTP credentials and server information to connect to [Aloes backend](https://framagit.org/aloes/device-manager).


## Requirements

### Arduino tools

Arduino IDE - download the latest from arduino

- https://www.arduino.cc/en/Main/Software

Or Arduino CLI - download the latest from arduino

- https://github.com/arduino/arduino-cli


### For ESP8266 

Packages for ESP8266 on Arduino

- Enter `http://arduino.esp8266.com/stable/package_esp8266com_index.json` into Additional Board Manager URLs field.

Packages to upload data on ESP8266 FileSystem

- Follow instructions on `https://github.com/esp8266/arduino-esp8266fs-plugin` to install the plugin in your Arduino IDE.


### For ESP32 

Packages for ESP32 on Arduino

- Enter `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` into Additional Board Manager URLs field.

Packages to upload data on ESP32 FileSystem

- Follow instructions on `https://github.com/me-no-dev/arduino-esp32fs-plugin` to install the plugin in your Arduino IDE.

### Dependencies

Requires the next libraries :  
- <Arduino.h>
- <advancedSerial.h>
- <Wire.h>
- <Ticker.h>
- <Bounce2.h>
- <time.h>
- <WiFiManager.h>
- on ESP8266 :
	- <ESP8266WiFi.h>
	- <ESP8266WiFiMulti.h>
	- <ESP8266mDNS.h>
- on ESP32 :
	- <ESPmDNS.h>
	- <WiFi.h>
	- <WiFiMulti.h>

## Installation

- Via git, clone into your `~/Arduino/Libraries` folder
```bash 
git clone https://framagit.org/aloes/arduino-network-manager.git 
```

## Usage

- This library should be combined with AloesDevice.h

- Open any .ino file of your project folder with Arduino IDE

- Configure WiFi credentials `config.h` at the root of your Arduino project

- Enter wifi and device credentials or configure the board via the Access Point ( 192.168.244.1 )

- Upload the code on your ESP8266 board


