#define MQTT_MAX_PACKET_SIZE 5000

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
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "consts.h"
#include "utils.h"

#include "debug.h"

#include "pref.h"
#include "dmx.h"
#include "inputs.h"
#include "commander.h"
#include "mqtt.h"
#include "network.h"
#include "display.h"
#include "neotrellis.h"
#include "encoder.h"
#include "webserver.h"

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
  pinMode(POT_2_PIN, INPUT);
  pinMode(POT_3_PIN, INPUT);
  pinMode(POT_4_PIN, INPUT);
  pinMode(POT_5_PIN, INPUT);
  Serial.println("ok");
 
  updateAnalogReadParameters();

  dmx_setup();

  display_print("WIFI");
  wifi_connect();

  display_print(".WWW");
  ws_setup();

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
