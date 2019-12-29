#include "config.h"

#if CLIENT_SECURE == 1
#include "secure-credentials.h"
#endif

#include <AloesDevice.h>
#include <NetworkManager.h>

Manager *manager = 0;
Network _network;

bool executeOnce = false;
bool wifiResetConfig = false;

// ADD YOUR LOGIC HERE

//  CALLED on errors
void onError(modules moduleName, char *error) {
  if (moduleName == TRANSPORT) {
    aSerial.vv().p(F("[TRANSPORT] error : ")).pln(error);
  }
}

// CALLED on incoming http/mqtt/serial message
// MQTT pattern = "prefixedDevEui/+method/+objectId/+sensorId/+resourceId",
// HTTP pattern = "apiRoot/+collection/+path/#param"
void onMessage(transportLayer type, Message *message) {
  if (type == MQTT) {
    char *payload = message->get(PAYLOAD);
    const char *method = message->get(METHOD);
    const char *objectId = message->get(OBJECT_ID);
    const char *resourceId = message->get(RESOURCE_ID);
    const char *nodeId = message->get(NODE_ID);
    const char *sensorId = message->get(SENSOR_ID);
    if (strcmp(objectId, "3306") == 0 && strcmp(resourceId, "5850") == 0) {
      if (strcmp(method, "1") == 0) {
        if ((strcmp(payload, "true") == 0 || strcmp(payload, "1") == 0)) {
          digitalWrite(RELAY_SWITCH, HIGH);
          digitalWrite(STATE_LED, LOW);
        } else if ((strcmp(payload, "false") == 0 ||
                    strcmp(payload, "0") == 0)) {
          digitalWrite(RELAY_SWITCH, LOW);
          digitalWrite(STATE_LED, HIGH);
        }
      } else if (strcmp(method, "2") == 0) {
        int val = digitalRead(RELAY_SWITCH);
        char newPayload[10];
        itoa(val, newPayload, 10);
        message->set(METHOD, "1").set(PAYLOAD, newPayload);
        aloes.sendMessage(MQTT);
      }
    }
    return;
  } else if (type == HTTP) {
    const char *method = message->get(METHOD);
    const char *collection = message->get(COLLECTION);
    const char *path = message->get(PATH);
    //  const char* body = message->get(PAYLOAD);
    //  aSerial.vvv().p(F("onMessage : HTTP : ")).p(collection).p("
    //  ").pln(path);
    return;
  }
}

void setup() {
  if (!aloesSetup()) {
    return;
  }
  if (wifiResetConfig) { // add a button ?
    WiFiManager wifiManager;
    wifiManager.resetSettings();
  }

  randomSeed(micros());
  helpers.setPins();
  helpers.startTick(1.5);

  if (!_network.setup(aloes.device->get(DEVICE_NAME))) {
    onErrorCallback(EXEC, (char*)"Failed to connect to WiFi AP, update your
    configuration.");
  }
  manager = new Manager(aloes.device);
  manager->setup();

  helpers.stopTick();
  // YOUR SETUP CODE
}

void loop() {
  if (!executeOnce && _network.connected()) {
    aSerial.v().println(F("====== Loop started ======"));
    randomSeed(micros());
    _network.setClock();
    executeOnce = true;
  }

  helpers.loop();
  manager->loop(aloes.device);

  if (!_network.connected()) {
    aloes.client->disconnect(MQTT);
    MilliSec lastWifiConnAttempt = millis();
    static AsyncWait asyncWifi;
    if (_network.asyncConnect(&asyncWifi, lastWifiConnAttempt, 1000)) {
      return;
    }
  }

  if (!_network.loop()) {
    aloes.client->disconnect(MQTT);
    return;
  }

  if (!aloesLoop()) {
    return;
  }
  // YOUR LOOP CODE
}