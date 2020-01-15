//#define DISABLE_MQTT

#define MQTT_SERVER "192.168.1.94"
//#define MQTT_SERVER "192.168.200.102"
#define MQTT_PORT 1010 // 1883

#define MQTT_TOPIC_RESPONSE "esp32/response"
#define MQTT_TOPIC_COMMANDS "esp32/commands"
#define MQTT_TOPIC_TEST "esp32/test"
#define MQTT_TOPIC_CONFIGURE_BUTTON_COMMAND "esp32/config/button/command/#"
#define MQTT_TOPIC_CONFIGURE_BUTTON_COLOR "esp32/config/button/color/#"
#define MQTT_TOPIC_RESET_BUTTON "esp32/config/button/rst/#"
#define MQTT_TOPIC_CONFIGURE_PARAMETER "esp32/config/param/#"

// Those are just for parsing
#define COLOR_BUTTON_TOPIC_NAME "esp32/config/button/color/"
#define COMMAND_BUTTON_TOPIC_NAME "esp32/config/button/command/"
#define RESET_BUTTON_TOPIC_NAME "esp32/config/button/rst/"
#define CONFIGURE_PARAMETER_TOPIC_NAME "esp32/config/param/"

#define MQTT_CHANNEL_BLINK_ON_TIME_DEFAULT 1000
#define MQTT_CHANNEL_BLINK_OFF_TIME_DEFAULT 500
int mqttChannelBlinkOnTime = MQTT_CHANNEL_BLINK_ON_TIME_DEFAULT;
int mqttChannelBlinkOffTime = MQTT_CHANNEL_BLINK_OFF_TIME_DEFAULT;

int neotrellis_colors[8 * 4];
int commandsCount = 0;
String neotrellis_commands[8 * 4][2]; // [ buttonIndex, Command ]

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void update_mqtt_cache()
{
  debug("    Updating cache...");
  mqttChannelBlinkOnTime = preferences.getInt(PREF_DISPLAY_BLINK_ON, MQTT_CHANNEL_BLINK_ON_TIME_DEFAULT);
  mqttChannelBlinkOffTime = preferences.getInt(PREF_DISPLAY_BLINK_OFF, MQTT_CHANNEL_BLINK_OFF_TIME_DEFAULT);
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  String topicText = String(topic);

  if (topicText.startsWith(COLOR_BUTTON_TOPIC_NAME))
  {
    preferences.begin(PREF_KEY, false);

    String buttonIdRaw = topicText.substring(String(COLOR_BUTTON_TOPIC_NAME).length());
    int buttonId = buttonIdRaw.toInt();
    int color = messageTemp.toInt();
    neotrellis_colors[buttonId] = color;

    String prefKeyStr = "btnColor" + buttonIdRaw;
    char prefKey[prefKeyStr.length() + 1];
    prefKeyStr.toCharArray(prefKey, prefKeyStr.length() + 1);

    preferences.putInt(prefKey, color);
    Serial.println("Changing color of button #" + buttonIdRaw + " to " + String(color, HEX));

    preferences.end();
  }
  else if (topicText.startsWith(COMMAND_BUTTON_TOPIC_NAME))
  {
    String buttonIdRaw = topicText.substring(String(COMMAND_BUTTON_TOPIC_NAME).length());
    int buttonId = buttonIdRaw.toInt();
    neotrellis_commands[commandsCount][0] = buttonIdRaw;
    neotrellis_commands[commandsCount][1] = messageTemp;

    Serial.println("Adding new listener for button #" + buttonIdRaw + " with command: \"" + messageTemp + "\"");
    debug("  Initializing preferences...");
    preferences.begin(PREF_KEY, false);

    /* STORE IN PREFERENCES */
    debug("  Converting button pref key...");
    String prefButtonKeyStr = "bC" + String(commandsCount) + "Btn";
    char buttonIdPrefKey[prefButtonKeyStr.length() + 1];
    prefButtonKeyStr.toCharArray(buttonIdPrefKey, prefButtonKeyStr.length() + 1);

    debug("  Converting command pref key...");
    String prefButtonCommandKeyStr = "bC" + String(commandsCount) + "Cmd";
    char buttonCommandPrefKey[prefButtonCommandKeyStr.length() + 1];
    prefButtonCommandKeyStr.toCharArray(buttonCommandPrefKey, prefButtonCommandKeyStr.length() + 1);

    debug("  Storing values...");
    preferences.putString(buttonCommandPrefKey, messageTemp);
    preferences.putInt(buttonIdPrefKey, buttonId);
    preferences.putInt(PREF_COMMAND_COUNT, commandsCount + 1);
    /* END STORING */

    commandsCount++;

    debug("  Closing preferences...");
    preferences.end();
  }
  else if (topicText.startsWith(RESET_BUTTON_TOPIC_NAME))
  {
    String buttonIdRaw = topicText.substring(String(RESET_BUTTON_TOPIC_NAME).length());
    int buttonId = buttonIdRaw.toInt();

    Serial.print("Resetting button #" + buttonIdRaw + "...");
    int resultingCommandsCount = commandsCount;
    debugln();
    debug("  Initializing preferences...");
    preferences.begin(PREF_KEY, false);

    debug("  Searching for stored command listeners...");
    for (int c = 0; c < commandsCount; c++)
    {
      String cmdBtnIdRaw = neotrellis_commands[c][0];
      int cmdButtonId = cmdBtnIdRaw.toInt();
      if (cmdButtonId == buttonId)
      {
        debug("    Found command listener at index #" + String(c) + ", popping it out.");
        // This is for popping the element from the array
        for (int i = c; i < resultingCommandsCount; i++) // Iterate through the rest of the array
          if (i < commandsCount)
          { // Check for not getting out of the array boundries
            debug("    Moving element #" + String(i + 1) + " to position #" + String(i));
            neotrellis_commands[i][0] = neotrellis_commands[i + 1][0]; // Place the next item into the current index
            neotrellis_commands[i][1] = neotrellis_commands[i + 1][1];

            debug("    Converting button pref key...");
            String prefButtonKeyStr = "bC" + String(i) + "Btn";
            char buttonIdPrefKey[prefButtonKeyStr.length() + 1];
            prefButtonKeyStr.toCharArray(buttonIdPrefKey, prefButtonKeyStr.length() + 1);

            debug("    Converting command pref key...");
            String prefButtonCommandKeyStr = "bC" + String(i) + "Cmd";
            char buttonCommandPrefKey[prefButtonCommandKeyStr.length() + 1];
            prefButtonCommandKeyStr.toCharArray(buttonCommandPrefKey, prefButtonCommandKeyStr.length() + 1);

            debug("    Storing changes...");
            preferences.putString(buttonCommandPrefKey, neotrellis_commands[i + 1][1]);
            preferences.putInt(buttonIdPrefKey, neotrellis_commands[i + 1][0].toInt());
          }
        resultingCommandsCount--; // Reduce the stored commands counter
        debug("    Now there are " + String(resultingCommandsCount) + " stored command listeners.");
      }
    }
    commandsCount = resultingCommandsCount;
    preferences.putInt(PREF_COMMAND_COUNT, commandsCount);

    debug("  Resetting color...");
    neotrellis_colors[buttonId] = 0xffffff;

    debug("  Closing preferences...");
    preferences.end();
    ndebug("ok");
  }
  else if (topicText.startsWith(CONFIGURE_PARAMETER_TOPIC_NAME))
  {
    String parameter = topicText.substring(String(CONFIGURE_PARAMETER_TOPIC_NAME).length());

    Serial.print("Storing \"" + messageTemp + "\" as \"" + parameter + "\"...");
    debugln();
    debug("  Initializing preferences...");
    preferences.begin(PREF_KEY, false);

    if (isValidKey(parameter))
    {
      debug("  Parameter is valid, storing...");
      String keyType = getKeyType(parameter);

      debug("    Converting key...");
      char prefKey[parameter.length() + 1];
      parameter.toCharArray(prefKey, parameter.length() + 1);

      debug("    Storing...");
      if (keyType == PREF_KEY_TYPE_STRING)
        preferences.putString(prefKey, messageTemp);
      else if (keyType == PREF_KEY_TYPE_INTEGER)
        preferences.putInt(prefKey, messageTemp.toInt());

      update_mqtt_cache();
    }
    else
      debug("  Invalid parameter: Key doesn't exist.");

    ndebug("ok");

    debug("  Closing preferences...");
    preferences.end();
  }
}

void mqtt_connect()
{
#ifdef DISABLE_MQTT
  Serial.println("  MQTT is disabled.");
#else
  Serial.print("  Setting up MQTT...");
  debug_("\n    Setting server \"");
  debug_(MQTT_SERVER);
  debug("\"...");
  mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
  debug("    Setting callback...");
  mqtt_client.setCallback(callback);

  debug("    Loading preferences...");
  debug("      Initializing...");
  preferences.begin(PREF_KEY, false);

  debug_("  ");
  update_mqtt_cache();

  debug("      Closing...");
  preferences.end();
#endif
}

void mqtt_reconnect()
{
#ifndef DISABLE_MQTT
  // Loop until we're reconnected
  while (!mqtt_client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt_client.connect("ESP32Client"))
    {
      Serial.println("connected");
      // Subscribe
      mqtt_client.subscribe(MQTT_TOPIC_TEST);
      mqtt_client.subscribe(MQTT_TOPIC_CONFIGURE_BUTTON_COLOR);
      mqtt_client.subscribe(MQTT_TOPIC_CONFIGURE_BUTTON_COMMAND);
      mqtt_client.subscribe(MQTT_TOPIC_RESET_BUTTON);
      mqtt_client.subscribe(MQTT_TOPIC_CONFIGURE_PARAMETER);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
#endif
}

void mqtt_loop()
{
#ifndef DISABLE_MQTT
  if (!mqtt_client.connected())
  {
    mqtt_reconnect();
  }
  mqtt_client.loop();
#endif
}

void mqtt_publish(String topic, String message)
{
#ifndef DISABLE_MQTT
  char atopic[topic.length() + 1];
  topic.toCharArray(atopic, topic.length() + 1);

  char amessage[message.length() + 1];
  message.toCharArray(amessage, message.length() + 1);

  mqtt_client.publish(atopic, amessage);
#endif
}
