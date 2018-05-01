/**
 * Workshop example for a remote control LED.
 *
 * Visit https://www.losant.com/kit for full instructions.
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 */

#include <ESP8266WiFi.h>
#include <Losant.h>

// Home WiFi credentials.
//const char* WIFI_SSID = "MySpectrumWiFi90-2G";
//const char* WIFI_PASS = "happyocean254";

// School WiFi credentials.
const char* WIFI_SSID = "xavierguest";
const char* WIFI_PASS = "";


// Losant credentials.
const char* LOSANT_DEVICE_ID = "5adb6a62e304ec0006542b16";
const char* LOSANT_ACCESS_KEY = "5e9d949d-161c-4ddf-a084-28756ab0ed0b";
const char* LOSANT_ACCESS_SECRET = "bdc5113ac11157138ba1a7ec4d45f73f5b13dba93876acdd91830e4b9b848184";

const int LED_PIN = 4;

bool ledState = false;

// For an unsecure connection to Losant.
WiFiClientSecure wifiClient;

LosantDevice device(LOSANT_DEVICE_ID);

// Changes the state of the led
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
    delay(1500);
    toggle();
  }
}

// Connects to wifi and Losant
void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

   // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
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


// Set up the device
void setup() {
  Serial.begin(9600);

  // Giving it a little time because the serial monitor doesn't
  // immediately attach. Want the workshop that's running to
  // appear on each upload.
  delay(2000);

  Serial.println();
  Serial.println("Running Workshop 2 Firmware.");

  pinMode(LED_PIN, OUTPUT);

  // Register the command handler to be called when a command is received
  // from the Losant platform.
  device.onCommand(&handleCommand);

  connect();
}

// Runs the device
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
