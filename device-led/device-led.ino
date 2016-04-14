/**
 * Example LED device for Losant blog. This sketch was designed
 * to run an Arduino 101 with an Arduino 101 WiFi Shield.
 *
 * Copyright (c) 2016 Losant. All rights reserved.
 * https://www.losant.com
 */

#include <WiFi101.h>
#include <Losant.h>

// WiFi credentials.
const char* WIFI_SSID = "wifi-ssid";
const char* WIFI_PASS = "wifi-pass";

// Losant credentials.
const char* LOSANT_DEVICE_ID = "my-device-id";
const char* LOSANT_ACCESS_KEY = "my-access-key";
const char* LOSANT_ACCESS_SECRET = "my-access-secret";

const int LED_PIN = 10;

bool ledState = false;

WiFiSSLClient wifiClient;

// Create a LosantDevice instance that will be used
// to communicate with the Losant platform.
LosantDevice device(LOSANT_DEVICE_ID);

void toggle() {
  Serial.println("Toggling LED.");
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// Called whenever the device receives a command from the Losant platform.
void handleCommand(LosantCommand *command) {
  Serial.print("Command received: ");
  Serial.println(command->name);

  if(strcmp(command->name, "toggle") == 0) {
    toggle();
  }
}

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to Losant.
  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  while(!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
  Serial.println();
  Serial.println("This device is now ready for use!");
}

void setup() {
  Serial.begin(115200);
  while(!Serial) { }
  
  pinMode(LED_PIN, OUTPUT);

  // Register the command handler to be called when a command is received
  // from the Losant platform.
  device.onCommand(&handleCommand);

  connect();
}

void loop() {

  bool toReconnect = false;

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi");
    toReconnect = true;
  }

  if(!device.connected()) {
    Serial.println("Disconnected from Losant");
    Serial.println(device.mqttClient.state());
    toReconnect = true;
  }

  if(toReconnect) {
    connect();
  }

  device.loop();

  delay(100);
}
