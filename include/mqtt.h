#define DISABLE_MQTT

#define MQTT_SERVER "192.168.1.94"

#define MQTT_TOPIC_RESPONSE "esp32/response"
#define MQTT_TOPIC_TEST "esp32/test"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

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
  mqtt_client.setServer(MQTT_SERVER, 1883);
  debug("    Setting callback...");
  mqtt_client.setCallback(callback);
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
      mqtt_client.subscribe("esp32/test");
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
