//#define DISABLE_MQTT

//#define MQTT_SERVER "192.168.0.101"

#define MQTT_SERVER "192.168.1.94"
//#define MQTT_SERVER "192.168.200.102"
#define MQTT_PORT 1010 // 1883

#define MQTT_CHANNEL_BLINK_ON_TIME_DEFAULT 1000
#define MQTT_CHANNEL_BLINK_OFF_TIME_DEFAULT 500

#define MQTT_DEVICE_NAME "ESP32Client"


#define DISPLAY_ADDRESS 0x70


//#define WIFI_SSID "AU_1_16_IDIOMES"
//#define WIFI_PASSWORD "alumnes1920"

//#define WIFI_SSID "AULA_TECNOLOGIA"
//#define WIFI_PASSWORD "alumnes1920"

//#define WIFI_SSID "AGORA_LAB_WIFI"
//#define WIFI_PASSWORD "EcWnLsNd2017"

#define WIFI_SSID "Wifi-Casa 2.4GHz"
#define WIFI_PASSWORD "rgo74amm75amg02rmg07"


#define DEBUG


#define POT_1_PIN 4
#define POT_2_PIN 15
#define POT_3_PIN 34
#define POT_4_PIN 35
#define POT_5_PIN 26

#define potCount 5
int potPins[potCount] = { POT_1_PIN, POT_2_PIN, POT_3_PIN, POT_4_PIN, POT_5_PIN };
int potReadings[potCount];


#define Y_DIM 8 //number of rows of key
#define X_DIM 4 //number of columns of keys

#define NEOTRELLIS_COUNT Y_DIM *X_DIM