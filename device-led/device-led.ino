/**
 * Example LED device for Structure blog. This sketch was designed
 * to run an Arduino 101 with an Arduino 101 WiFi Shield.
 *
 * Copyright (c) 2016 Structure. All rights reserved.
 * http://www.getstructure.io
 */

#include <WiFi101.h>
#include <Structure.h>

// WiFi credentials.
const char* WIFI_SSID = "wifi-ssid";
const char* WIFI_PASS = "wifi-pass";

// Structure credentials.
const char* STRUCTURE_DEVICE_ID = "my-device-id";
const char* STRUCTURE_ACCESS_KEY = "my-access-key";
const char* STRUCTURE_ACCESS_SECRET = "my-access-secret";

const int LED_PIN = 10;

bool ledState = false;

WiFiSSLClient wifiClient;

// Create a StructureDevice instance that will be used
// to communicate with the Structure platform.
StructureDevice device(STRUCTURE_DEVICE_ID);

void toggle() {
  Serial.println("Toggling LED.");
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
}

// Called whenever the device receives a command from the Structure platform.
void handleCommand(StructureCommand *command) {
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

  // Connect to Structure.
  Serial.println();
  Serial.print("Connecting to Structure...");

  device.connectSecure(wifiClient, STRUCTURE_ACCESS_KEY, STRUCTURE_ACCESS_SECRET);

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
  // from the Structure platform.
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
    Serial.println("Disconnected from Structure");
    Serial.println(device.mqttClient.state());
    toReconnect = true;
  }

  if(toReconnect) {
    connect();
  }

  device.loop();

  delay(100);
}
