
#define POT_1_PIN 4
#define POT_2_PIN 15
#define POT_3_PIN 34
#define POT_4_PIN 35
#define POT_5_PIN 26

#define potCount 5
int potPins[potCount] = { POT_1_PIN, POT_2_PIN, POT_3_PIN, POT_4_PIN, POT_5_PIN };
int potReadings[potCount];

void makeAnalogReadings(){
  for (int c = 0; c < potCount; c++) {
    int pin = potPins[c];
    int read = analogRead(pin);
    potReadings[c] = read;
  }
}

void sendAnalogReadValues() {
  for (int c = 0; c < potCount; c++) {
    int read = potReadings[c];
    if (mqtt_client.connected()) {
      String topic = "esp32/reading/pot" + String(c);
      mqtt_publish(topic, String(read));
    }
  }
}

void updateAnalogReadParameters() {
  Serial.print("    Setting read parameters from preferences...");
  debug("\n      Initializing preferences");
  preferences.begin(PREF_KEY, false);

  debug("      Setting analog read resolution");
  int analogReadRes = readPref(PREF_ANALOG_READ_RESOLUTION, 12);
  if (analogReadRes >= 9 && analogReadRes <= 12) {
    debug("        Set to (%s)\n", (String) analogReadRes);
    analogReadResolution(analogReadRes);
  } else
    debug("        Invalid stored value (%s)\n", (String) analogReadRes);

  debug("      Setting analog read samples");
  int analogReadSam = readPref(PREF_ANALOG_READ_SAMPLES, 1);
  analogSetSamples(analogReadSam);
  debug("        Set to (%s)\n", (String) analogReadSam);

  debug("      Closing preferences");
  preferences.end();
  ndebug("ok");
}
