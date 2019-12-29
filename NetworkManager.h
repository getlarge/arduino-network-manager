#ifndef NetworkManager_h
#define NetworkManager_h

#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#endif

#include <advancedSerial.h>

#include <Bounce2.h>
#include <Ticker.h>

#include <WiFiManager.h>
#include <time.h>

#include "lib/AsyncWait.h"

bool callConfigMode = false;
bool manualConfig = false;

#include "lib/Helpers.cpp"
Helpers helpers;

#include "lib/Network.cpp"
// Network _network;

#include "lib/Manager.cpp"
// Manager manager(device);

#endif
