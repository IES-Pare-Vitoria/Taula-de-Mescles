void processSerialCommand(String serialRead) {
  serialRead.replace("\n", "");
  if (serialRead.startsWith("CF")) { // Set config value
    serialRead = serialRead.substring(3, serialRead.length());
    String key = "";
    for (int c = 0; c < serialRead.length(); c++)
      if (serialRead.charAt(c) == ' ')
        break;
      else
        key += serialRead.charAt(c);

    String value = serialRead.substring(key.length() + 1, serialRead.length());

    if (isValidKey(key)) {
      String type = getKeyType(key);

      if (type == PREF_KEY_TYPE_INTEGER) {
        int intValue = value.toInt();
        debug("Initializing preferences");
        preferences.begin(PREF_KEY, false);

        char keyArray[key.length() + 1];
        key.toCharArray(keyArray, key.length() + 1);

        Serial.print("Setting \"");
        Serial.print(keyArray);
        Serial.print("\" to ");
        Serial.println(intValue);
        preferences.putInt(keyArray, intValue);

        debug("  Closing preferences");
        preferences.end();
        ndebug("ok");
      }

      updateAnalogReadParameters();
    } else {
      Serial.println("Invalid key sent");
    }
  } else if (serialRead.startsWith("IP")) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Wifi is connected with IP: ");
      Serial.println(WiFi.localIP());   //inform user about his IP address
    } else {
      Serial.println("WiFi is not connected.");
    }
  } else if (serialRead.startsWith("AR")) { // Analog Read
    if(serialRead.length() <= 3){
      Serial.println("A channel is required. Usage: \"AR [pot_channel]\"");
      return;
    }
    serialRead = serialRead.substring(3, serialRead.length());
    int index = serialRead.toInt();
    int pin = potPins[index];
    int read = analogRead(potPins[index]);
    Serial.print("Read pin #");
    Serial.print(pin);
    Serial.print(": ");
    Serial.println(read);
  } else if (serialRead.startsWith("DC")) { // DMX Channel
    if(serialRead.length() <= 3){
      Serial.println("A channel is required. Usage: \"DC [channel]\"");
      return;
    }
    serialRead = serialRead.substring(3, serialRead.length());
    int channel = serialRead.toInt();
    currentDMXChannel = channel;
    Serial.print("New DMX Channel: ");
    Serial.println(channel);
  } else if (serialRead.startsWith("DE")) { // DMX Debug Enable
    if(serialRead.length() <= 3){
      Serial.println("A channel is required. Usage: \"DE [channel]\"");
      return;
    }
    serialRead = serialRead.substring(3, serialRead.length());
    int channel = serialRead.toInt();
    debuggingChannel[channel] = !debuggingChannel[channel];
    Serial.print("Debugging DMX channel #");
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(debuggingChannel[channel]);
  }
}
