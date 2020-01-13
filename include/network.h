//#define EAP_AUTH
#define EAP_IDENTITY "al037092" //if connecting from another corporation, use identity@organisation.domain in Eduroam 
#define EAP_PASSWORD "10495262" //your Eduroam password

#define WIFI_SSID "AU_1_16_IDIOMES"
#define WIFI_PASSWORD "alumnes1920"

//#define WIFI_SSID "Wifi-Casa 2.4GHz"
//#define WIFI_PASSWORD "rgo74amm75amg02rmg07"

int wifi_counter = 0;

void wifi_connect() {
  Serial.print("  Setting up WiFi...");
  debug("");
  debug("    Disconnecting WiFi...");
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  debug("    Setting WiFi mode to STA...");
  WiFi.mode(WIFI_STA); //init wifi mode

#ifdef EAP_AUTH
  debug("    Setting EAP Identity...");
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
  debug("    Setting EAP Username...");
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username --> identity and username is same
  debug("    Setting EAP Password...");
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  debug("    Setting WPA2 Config Settings...");
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  debug("    Enabling WPA Enterprise...");
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
#endif

  debug("    Starting WiFi...");
#ifdef WIFI_PASSWORD
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#else
  WiFi.begin(WIFI_SSID);
#endif

  debug_("    Connecting");
  wifi_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    debug_(".");
    wifi_counter++;
    if (wifi_counter >= 60) { //after 30 seconds timeout - reset board
      Serial.println("Could not connect to the network!");
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("    WiFi connected");
  Serial.print("    IP address set: ");
  Serial.println(WiFi.localIP()); //print LAN IP
}

void wifi_check() {
  if (WiFi.status() == WL_CONNECTED) { //if we are connected to Eduroam network
    wifi_counter = 0; //reset counter
    //debug_("Wifi is still connected with IP: ");
    //debug(WiFi.localIP());   //inform user about his IP address
  } else if (WiFi.status() != WL_CONNECTED) { //if we lost connection, retry
#ifdef WIFI_PASSWORD
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#else
    WiFi.begin(WIFI_SSID);
#endif
  }
  while (WiFi.status() != WL_CONNECTED) { //during lost connection, print dots
    delay(500);
    Serial.print(".");
    wifi_counter++;
    if (wifi_counter >= 60) { //30 seconds timeout - reset board
      ESP.restart();
    }
  }
}
