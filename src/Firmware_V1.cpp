#include <Preferences.h>
#include <WiFi.h> //Wifi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <PubSubClient.h>// https://github.com/knolleary/pubsubclient/archive/master.zip
#include "ESPDMX.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "AiEsp32RotaryEncoder.h"
#include "Adafruit_NeoTrellis.h"

#define DEBUG

#define POT_1_PIN 4
#define POT_2_PIN 15
#define POT_3_PIN 34
#define POT_4_PIN 35
#define POT_5_PIN 26

#define potCount 5
int potPins[potCount] = { POT_1_PIN, POT_2_PIN, POT_3_PIN, POT_4_PIN, POT_5_PIN };
int potReadings[potCount];

#include "debug.h"

#include "pref.h"
#include "dmx.h"
#include "mqtt.h"
#include "inputs.h"
#include "network.h"
#include "commander.h"
#include "display.h"
#include "neotrellis.h"
#include "encoder.h"

unsigned long lastWifiCheck = 0;
unsigned long lastReadSent = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting up Control Panel...");

  Serial.println("  Initializing display...");
  display_init();
  Serial.println("    Showing Boot message...");
  display_print("BOOT");

  Serial.println("  Configuring IO");
  Serial.print("    Setting inputs...");
  pinMode(POT_1_PIN, INPUT);
  Serial.println("ok");

  updateAnalogReadParameters();

  dmx_setup();

  display_print("WIFI");
  wifi_connect();

  display_print("MQTT");
  mqtt_connect();

  neotrellis_setup();

  encoder_setup();

  display_print("IDLE");
  Serial.println("SYSTEM READY");
  Serial.println();
}

void loop() {
  mqtt_loop();
  
  if (Serial.available())
    processSerialCommand(Serial.readString());

  if(millis() - lastWifiCheck >= 10000) {
    wifi_check();
    lastWifiCheck = millis();
  }

  makeAnalogReadings();
  if(millis() - lastReadSent >= 3000){
    sendAnalogReadValues();
    lastReadSent = millis();
  }

  rotary_loop();

  neotrellis_loop();

  display_loop();

  dmx_tick();
}
