
#define DISPLAY_ADDRESS 0x70

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

unsigned long lastMQTTChannelBlink = 0;

bool settingMQTTChannel = false;

void display_init()
{
    alpha4.begin(DISPLAY_ADDRESS);
}

void display_print(String message)
{
    //debug("Showing " + message + " in the display.");

    if (message.length() > 0)
        alpha4.writeDigitAscii(0, message.charAt(0));
    if (message.length() > 1)
        alpha4.writeDigitAscii(1, message.charAt(1));
    if (message.length() > 2)
        alpha4.writeDigitAscii(2, message.charAt(2));
    if (message.length() > 3)
        alpha4.writeDigitAscii(3, message.charAt(3));

    alpha4.writeDisplay();
}

void display_loop()
{
    if (settingMQTTChannel)
        if (millis() - lastMQTTChannelBlink > mqttChannelBlinkOnTime + mqttChannelBlinkOffTime)
            lastMQTTChannelBlink = millis();
        else if(millis() - lastMQTTChannelBlink > mqttChannelBlinkOnTime)
            display_print("C   ");
        else
        {
            // TODO: Make function
            String channel = "C";
            if (currentDMXChannel < 10)
                channel += "00";
            else if (currentDMXChannel < 100)
                channel += "0";
                
            channel += String(currentDMXChannel);

            display_print(channel);
        }
        else if(currentDMXChannel > 0){
            String channel = "C";
            if (currentDMXChannel < 10)
                channel += "00";
            else if (currentDMXChannel < 100)
                channel += "0";
                
            channel += String(currentDMXChannel);

            display_print(channel);
        }else display_print("IDLE");
}