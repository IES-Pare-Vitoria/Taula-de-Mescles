void processSerialCommand(String serialRead)
{
  serialRead.replace("\n", "");
  if (serialRead.startsWith("CF"))
  { // Set config value
    serialRead = serialRead.substring(3, serialRead.length());
    String key = "";
    for (int c = 0; c < serialRead.length(); c++)
      if (serialRead.charAt(c) == ' ')
        break;
      else
        key += serialRead.charAt(c);

    String value = serialRead.substring(key.length() + 1, serialRead.length());

    if (isValidKey(key))
    {
      String type = getKeyType(key);

      if (type == PREF_KEY_TYPE_INTEGER)
      {
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

#ifdef updateAnalogReadParameters
      updateAnalogReadParameters();
#endif
    }
    else
    {
      Serial.println("Invalid key sent");
    }
  }
  else if (serialRead.startsWith("IP"))
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("Wifi is connected with IP: ");
      Serial.println(WiFi.localIP()); //inform user about his IP address
    }
    else
    {
      Serial.println("WiFi is not connected.");
    }
  }
  else if (serialRead.startsWith("AR"))
  { // Analog Read
    if (serialRead.length() <= 3)
    {
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
  }
  else if (serialRead.startsWith("DC"))
  { // DMX Channel
    if (serialRead.length() <= 3)
    {
      Serial.println("A channel is required. Usage: \"DC [channel]\"");
      return;
    }
    serialRead = serialRead.substring(3, serialRead.length());
    int channel = serialRead.toInt();
#ifdef currentDMXChannel
    currentDMXChannel = channel;
#endif
    Serial.print("New DMX Channel: ");
    Serial.println(channel);
  }
  else if (serialRead.startsWith("DE"))
  { // DMX Debug Enable
    if (serialRead.length() <= 3)
    {
      Serial.println("A channel is required. Usage: \"DE [channel]\"");
      return;
    }
    serialRead = serialRead.substring(3, serialRead.length());
    int channel = serialRead.toInt();
#ifdef debuggingChannel
    debuggingChannel[channel] = !debuggingChannel[channel];
    Serial.print("Debugging DMX channel #");
    Serial.print(channel);
    Serial.print(": ");
    Serial.println(debuggingChannel[channel]);
#endif
  }
}

void sendCommand(String command)
{
  if (command.startsWith("D")) // D##-## Channel-Value
  {
#ifdef dmx
    String channelRaw = "";
    int sepparatorIndex = command.indexOf('-');
    String channel = command.substring(1, sepparatorIndex);
    String value = command.substring(sepparatorIndex + 1, command.length());
    Serial.println("Writing to DMX #" + channel + " value " + value);
    debug("  Writing...");
    dmx.write(channel.toInt(), value.toInt());
    debug("  Updating...");
    dmx.update();
#else
    Serial.println("DMX disabled");
#endif
  }
  else if (command.startsWith("R")) // R##-## Channel-ChN : Resets the amount of channels specified
  {
#ifdef dmx
    String channelRaw = "";
    int sepparatorIndex = command.indexOf('-');
    String channel = command.substring(1, sepparatorIndex);
    String value = command.substring(sepparatorIndex + 1, command.length());
    int channelInt = channel.toInt();
    int valueInt = value.toInt();
    int endingChannel = channelInt + valueInt;
    for (int c = channelInt; c < endingChannel; c++)
    {
      Serial.println("Writing to DMX #" + String(c) + " value 0");
      debug("  Writing...");
      dmx.write(c, 0);
      debug("  Updating...");
      dmx.update();
    }
#else
    Serial.println("DMX disabled");
#endif
  }
  else if (command.startsWith("P"))
  { // P##-## Channel-Angle : For panning 540º Focus
#ifdef dmx
    String channelRaw = "";
    int sepparatorIndex = command.indexOf('-');
    String channel = command.substring(1, sepparatorIndex);
    String value = command.substring(sepparatorIndex + 1, command.length());
    int channelInt = channel.toInt();
    int valueInt = value.toInt();
    int parsedValue = (valueInt * 255) / 540;

    Serial.println("Writing to DMX #" + channel + " value " + String(parsedValue));
    debug("  Writing...");
    dmx.write(channelInt, parsedValue);
    debug("  Updating...");
    dmx.update();
#else
    Serial.println("DMX disabled");
#endif
  }
  else if (command.startsWith("T"))
  { // T##-## Channel-Angle : For tilting 180º Focus
#ifdef dmx
    String channelRaw = "";
    int sepparatorIndex = command.indexOf('-');
    String channel = command.substring(1, sepparatorIndex);
    String value = command.substring(sepparatorIndex + 1, command.length());
    int channelInt = channel.toInt();
    int valueInt = value.toInt();
    int parsedValue = (valueInt * 255) / 180;

    Serial.println("Writing to DMX #" + channel + " value " + String(parsedValue));
    debug("  Writing...");
    dmx.write(channelInt, parsedValue);
    debug("  Updating...");
    dmx.update();
#else
    Serial.println("DMX disabled");
#endif
  }
  else if (command.startsWith("W"))
  { // W## time : For waiting in ms
    int time = command.substring(1, command.length()).toInt();
    Serial.println("Waiting for " + String(time) + "ms");
    delay(time);
  }

  Serial.print("C-");
  Serial.println(command);
}