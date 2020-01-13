
#define DMX_CHANNELS 1

#define DMX_PIN 5

DMXESPSerial dmx;

int currentDMXChannel = 0;
bool debuggingChannel[DMX_CHANNELS] = { false };

void dmx_setup(){
  Serial.print("    Initializing DMX...");
  debug("");
  debug("      Setting Up DMX 1...");
  dmx.init(512, DMX_PIN);
  ndebug("ok");
}

void dmx_tick(){
  for(int c = 0; c < DMX_CHANNELS; c++){
    //debug_("Ticking dmx ");
    //debug(String(c));
    //debug_("  Reading...");
    int read = potReadings[c];
    //debug(String(read));
    //debug_("  Mapping...");
    int mapped = map(read, 0, 4095, 0, 255);
    //debug(String(mapped));
    //debug_("  Getting Channel...");
    int channel = currentDMXChannel + c;
    //debug(String(channel));
    if(debuggingChannel[c] == true){
      Serial.print("DMX Update. Channel: #");
      Serial.print(channel);
      Serial.print(" Value: ");
      Serial.println(mapped);
    }
    //debug("  Writing...");
    dmx.write(channel, mapped);
    //debug("  Updating...");
    dmx.update();
  }
}
