#include "Network.h"

// bool Network::reportErrors = false;

// void Network::setErrorCallback(ERROR_CALLBACK_SIGNATURE) {
//   reportErrors = true;
//   this->onError = onError;
// }

// void Network::setError(char *error) {
//   _error = error;
//   if (reportErrors && strcmp(error, "") != 0) {
//     onError(NETWORK, error);
//   }
// }

// void Network::setError(const char *error) { setError((char *)error); }

// Set time via NTP, as required for x.509 validation
void Network::setClock() {
  configTime(2 * 3600, 0, "0.fr.pool.ntp.org", "1.fr.pool.ntp.org");
  setClock_status = STARTED;
  aSerial.vv().pln(F("Waiting for NTP time sync "));
  setClock_AsyncWait.startWaiting(millis(), 1000); // Log every second.
}

// Check Clock Status and update 'setClock_status' accordingly.
void Network::checkClockStatus() {
  time_t now = time(nullptr);
  if (now < 8 * 3600 * 2 && _clockFailure < _maxClockFailure) {
    // The NTP request has not yet completed.
    if (!setClock_AsyncWait.isWaiting(millis())) {
      aSerial.vv().p(F("."));
      _clockFailure += 1;
      setClock_AsyncWait.startWaiting(millis(), 1000); // Log every second.
    }
    //  setError("Check clock status");
    return;
  } else if (_clockFailure >= _maxClockFailure) {
    setClock_status = FAILED;
    return;
  }

  setClock_status = SUCCESS;
  _clockFailure = 0;
  //  wifiClient.setX509Time(now);
#if DEBUG > 0
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  aSerial.vv().pln(F("")).p(F("Current time: ")).pln(asctime(&timeinfo));
#endif // DEBUG
}

void Network::checkSerial() {
  while (Serial.available() == 0 && millis() < 4000)
    ;
  // On timeout or availability of data, we come here.
  if (Serial.available() > 0) {
    // If data is available, we enter here.
    int test = Serial.read(); // We then clear the input buffer
    Serial.println("DEBUG");  // Give feedback indicating mode
    Serial.println(DEBUG);
    aSerial.on();
  } else {
    aSerial.off();
    // Serial.setDebugOutput(false);
  }
}

bool Network::isReady() { return (setClock_status > FINISHED); }

bool Network::connect() {
  //  IPAddress _ip, _gw, _sn;
  //  _ip.fromString(device.getStaticIp());
  //  _gw.fromString(device.getStaticGw());
  //  _sn.fromString(device.getSstaticSn());
#if defined(ESP8266)
  WiFi.hostname(_hostname);
#elif defined(ESP32)
  WiFi.setHostname(_hostname);
#endif

  WiFi.mode(WIFI_STA);
  //  WiFi.begin(ssid, password);
  wiFiMulti.addAP(_ssid, _password);
  // String ssid = WiFi.SSID();
  // String pass = WiFi.psk();

  if (connected()) {
    if (!MDNS.begin(_hostname)) {
      // setError("Failed to set up MDNS responder");
      return false;
    }
    wifiFailCount = 0;
    aSerial.vv()
        .p(F("[NETWORK] WiFi connected. IP Address : "))
        .pln(WiFi.localIP());
    // if (configMode == 1) {quitConfigMode();}
    //  MDNS.addService("http", "tcp", 80);
    return true;
  }
  // setError("Wifi connection failure");
  return false;
}

bool Network::asyncConnect(AsyncWait *async, MilliSec startTime,
                           unsigned long interval) {
  if (async->isWaiting(startTime)) {
    return true;
  }
  if (connect()) {
    wifiFailCount = 0;
    return false;
  }
  if (wifiFailCount > wifiMaxFailedCount && !callConfigMode) {
    wifiFailCount = 0;
    callConfigMode = true;
  }
  ++wifiFailCount;
  async->startWaiting(startTime, interval);
  return true;
}

bool Network::asyncConnect(AsyncWait *async, MilliSec startTime) {
  return asyncConnect(async, startTime, reconnectInterval);
}

bool Network::connected() {
  return (wiFiMulti.run() == WL_CONNECTED);
  //  return (WiFi.status() == WL_CONNECTED);
}

String Network::getMacAddress() {
  byte mac[6];
  String macStr;
  WiFi.macAddress(mac);
  macStr = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" +
           String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" +
           String(mac[4], HEX) + ":" + String(mac[5], HEX);

  return macStr;
}

bool Network::setup(char *hostname) {
  //  checkSerial();
  strlcpy(_hostname, hostname, sizeof(_hostname) - 1);

#ifdef DEFAULT_WIFI_SSID
  // _ssid = DEFAULT_WIFI_SSID;
  strlcpy(_ssid, DEFAULT_WIFI_SSID, sizeof(_ssid) - 1);
#else
  // _ssid = WiFi.SSID().c_str();
  strlcpy(_ssid, WiFi.SSID().c_str(), sizeof(_ssid) - 1);
#endif
#ifdef DEFAULT_WIFI_PASS
  // _password = DEFAULT_WIFI_PASS;
  strlcpy(_password, DEFAULT_WIFI_PASS, sizeof(_password) - 1);
#else
  // _password = WiFi.psk().c_str();
  strlcpy(_password, WiFi.psk().c_str(), sizeof(_password) - 1);
#endif

  aSerial.vv()
      .pln(F(""))
      .p(F("Network setup : "))
      .p(_ssid)
      .p(" ")
      .pln(_password);
  if (connect()) {
    return true;
  }
  // randomSeed(micros());
  // setClock();

  // #if MANUAL_SIGNING
  //   signPubKey = new BearSSL::PublicKey(pubkey);
  //   hash = new BearSSL::HashSHA256();
  //   sign = new BearSSL::SigningVerifier(signPubKey);
  // #endif

  // int numCerts = certStore.initCertStore(&certs_idx, &certs_ar);
  // client->setCertStore(&certStore);
  // Serial.println(numCerts);
  return false;
}

bool Network::loop() {
  // Prevent ALL other actions here until the clock as been set by NTP.
  if (setClock_status < FINISHED) {
    checkClockStatus();
    return false;
  }
  if (!isReady()) {
    return false;
  }
  return true;
}
