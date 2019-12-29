#ifndef Network_h
#define Network_h

class Network {
public:
  Network(){};
  virtual ~Network() {}
  //    ESP8266WiFiMulti & getWiFiClient() {
  //       return WiFiMulti;
  // }
  //	WiFiClient getWiFiClient();

  // void setErrorCallback(ERROR_CALLBACK_SIGNATURE);
  // void setError(char *error);
  // void setError(const char *error);

  bool isReady();
  void setClock();
  void checkSerial();

  bool asyncConnect(AsyncWait *async, MilliSec startTime);
  bool asyncConnect(AsyncWait *async, MilliSec startTime,
                    unsigned long interval);
  bool connect();
  bool connected();

  String getMacAddress();

  bool setup(char *hostname);
  bool loop();

  int wifiFailCount = 0;
  int wifiMaxFailedCount = 30;

private:
  // ERROR_CALLBACK_SIGNATURE;
  // const char *_error;
  // static bool reportErrors;

#if defined(ESP8266)
  ESP8266WiFiMulti wiFiMulti;
#elif defined(ESP32)
  WiFiMulti wiFiMulti;
#endif
  unsigned long reconnectInterval = 1000;
  // const char *_ssid;
  // const char *_password;
  // const char *_hostname;

  char _ssid[100];
  char _password[100];
  char _hostname[100];

  const char *signPubKey;
  const char *sign;

  // Note: variables of type 'status' are NEVER assigned a value of 'FINISHED'.
  // 'FINISHED' is only used in comparisons (e.g. if(x_status>FINISHED)... ),
  // rather than always testing for SUCCESS || FAILED.
  enum status { NOT_STARTED, STARTED, FINISHED, SUCCESS, FAILED };
  status setClock_status = NOT_STARTED;
  AsyncWait setClock_AsyncWait;

  int _maxClockFailure = 15;
  int _clockFailure = 0;
  void checkClockStatus();
};

#endif