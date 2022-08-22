/**
 * Uses v2.7.4 of ESP8266 in Board Manager
 * Use Board: NodeMCU 1.0 (ESP-12E Module)
 * Flash Size: 4 MB (FS:2MB OTA:~1019KB)
 * Recommended debug level: SSL+TLS_MEM+HTTP_CLIENT
 * 
 * Upload certs to SPIFFS memory using: https://github.com/esp8266/arduino-esp8266fs-plugin.
 * 
 * Flash requirements for MacOS:
 * custom patch to pyserial https://forum.arduino.cc/t/pyserial-and-esptools-directory-error/671804/5.
 * Maybe:
 * - ESP Exception Decoder
 * - pip3 install esptool https://pypi.org/project/esptool/.
 * - pip3 install pyserial https://pypi.org/project/pyserial/.
 * - MCU driver https://www.silabs.com/Support%20Documents/Software/Mac_OSX_VCP_Driver.zip.
 * - Flash download tool https://www.espressif.com/en/support/download/other-tools.
 * 
 * RGB LEDS: https://www.elegoo.com/collections/electronic-component-kits/products/elegoo-led-assortment-kit
 */

#include "fantypes.h"
#include "ledstates.h"
// defines THINGNAME
// exposes WIFI_SSID, WIFI_PASSWORD, and AWS_IOT_ENDPOINT
#include "secrets.h";

#include <time.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// The DER versions of certs issued by AWS
#define AWS_IOT_CA_FILE "/ca.der"
#define AWS_IOT_CERT_FILE "/cert.der"
#define AWS_IOT_PRIVATE_KEY_FILE "/private.der"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "master_bedroom/fan/receipt"
#define AWS_IOT_SUBSCRIBE_TOPIC "master_bedroom/fan/control"

#define SERIAL_BAUD_RATE 115200

bool isInError = false;
 
WiFiClientSecure net;
MQTTClient client = MQTTClient(256);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const int TX_PIN = D1;

// Set time via NTP, as required for x.509 validation
void setClock() {
  timeClient.begin();
  while(!timeClient.update()){
    timeClient.forceUpdate();
  }

  net.setX509Time(timeClient.getEpochTime());
}

void connectWiFi() {
  net.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  setLedState(LED_STATE_TRANSMITTING);
  while (WiFi.status() != WL_CONNECTED) {
    if (isLedOff()) {
      setLedState(LED_STATE_TRANSMITTING);
    } else {
      setLedState(LED_STATE_OFF);
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  setLedState(LED_STATE_OFF);

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  setClock();
}

void setupTLS() {
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());

   // Load certificate file
  File cert = SPIFFS.open(AWS_IOT_CERT_FILE, "r");
  if (!cert) {
    Serial.println("Failed to open cert file");
  } else {
    Serial.println("Success to open cert file");
  }

  delay(1000);

  if (net.loadCertificate(cert)) {
    Serial.println("cert loaded");
  } else {
    Serial.println("cert not loaded");
  }

  // Load private key file
  File private_key = SPIFFS.open(AWS_IOT_PRIVATE_KEY_FILE, "r");
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  } else {
    Serial.println("Success to open private cert file");
  }

  delay(1000);

  if (net.loadPrivateKey(private_key)) {
    Serial.println("private key loaded");
  } else {
    Serial.println("private key not loaded");
  }

  // Load CA file
  File ca = SPIFFS.open(AWS_IOT_CA_FILE, "r");
  if (!ca) {
    Serial.println("Failed to open ca ");
  } else {
    Serial.println("Success to open ca");
  }

  delay(1000);

  if (net.loadCACert(ca)) {
    Serial.println("ca loaded");
  } else {
    Serial.println("ca failed");
  }

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
}

void transmit(signal signalArray[16] = sig_preamble) {
  for (int burst = 0; burst < 6; burst++) {
    digitalWrite(LED_BUILTIN, LOW);
    setLedState(LED_STATE_TRANSMITTING);
    // Should add up to 50
    for (int pre = 0; pre < 32; pre++) {
      signal preambleSignal = sig_preamble[pre];
      digitalWrite(TX_PIN, preambleSignal.power);
      delayMicroseconds(preambleSignal.duration);
    }
    for (int msgIdx = 0; msgIdx < 10; msgIdx++) {
      signal messageSignal = signalArray[msgIdx];
      digitalWrite(TX_PIN, messageSignal.power);
      delayMicroseconds(messageSignal.duration);
    }
    for (int post = 0; post < 8; post++) {
      signal postambleSignal = sig_postamble[post];
      digitalWrite(TX_PIN, postambleSignal.power);
      delayMicroseconds(postambleSignal.duration);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    setLedState(LED_STATE_OFF);
  }
}

int fanControl(String cmd) {
  if (cmd == "light") {
    transmit(sig_light);
    return 0;
  }

  if (cmd == "off") {
    transmit(sig_power);
    return 0;
  }

  if (cmd == "speed1") {
    transmit(sig_speed_1);
    return 0;
  }

  if (cmd == "speed2") {
    transmit(sig_speed_2);
    return 0;
  }

  if (cmd == "speed3") {
    transmit(sig_speed_3);
    return 0;
  }

  return 1;
}

void publishMessage(String message, int result) {
  setLedState(LED_STATE_TRANSMITTING);
  StaticJsonDocument<200> doc;
  doc["timestamp"] = millis();
  doc["message"] = message;
  doc["result"] = result;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  setLedState(LED_STATE_RECEIVING);
  Serial.println("incoming: " + topic + " - " + payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  int result = fanControl(message);
  publishMessage(message, result);
  delay(1000);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (isLedOff()) {
      setLedState(LED_STATE_TRANSMITTING);
    } else {
      setLedState(LED_STATE_OFF);
    }
    // Attempt to connect
    if (client.connect(THINGNAME)) {
      setLedState(LED_STATE_POWER);
      Serial.println("AWS IoT Connected!");
      client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    } else {
      setLedState(LED_STATE_ERROR);
      Serial.print("failed, rc=");
      Serial.println(" try again in 5 seconds");

      char buf[256];
      net.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

const int TEST_INTERVAL = 500;
void testPeripherals() {
  setLedState(LED_STATE_POWER);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_OFF);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_ERROR);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_OFF);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_RECEIVING);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_OFF);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_TRANSMITTING);
  delay(TEST_INTERVAL);
  setLedState(LED_STATE_OFF);
  delay(TEST_INTERVAL);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(TX_PIN, OUTPUT);
  setupLeds();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


  Serial.begin(SERIAL_BAUD_RATE);
  testPeripherals();
  connectWiFi();
  
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    isInError = true;
    return;
  }

  setupTLS();

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);
}

void loop() {
  if (isInError) {
    setLedState(LED_STATE_ERROR);
  } else {
    setLedState(LED_STATE_POWER);
  }
  
  if (!client.connected()) {
    reconnect();
  }
 
  client.loop();
  delay(1000);
}
