
void makeAnalogReadings(){
  for (int c = 0; c < potCount; c++) {
    int pin = potPins[c];
    int read = analogRead(pin);
    potReadings[c] = read;
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
