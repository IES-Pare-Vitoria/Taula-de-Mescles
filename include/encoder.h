
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 33
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_VCC_PIN -1 /*put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);

void rotary_onButtonClick()
{
    Serial.println("Clicked button");
}

void rotary_loop()
{
    //first lets handle rotary encoder button click
    if (rotaryEncoder.currentButtonState() == BUT_RELEASED)
        //we can process it here or call separate function like:
        rotary_onButtonClick();

    //lets see if anything changed
    int16_t encoderDelta = rotaryEncoder.encoderChanged();

    //optionally we can ignore whenever there is no change
    if (encoderDelta == 0)
        return;

    //for some cases we only want to know if value is increased or decreased (typically for menu items)
    //   if (encoderDelta > 0) Serial.print("+");
    //   if (encoderDelta < 0) Serial.print("-");

    //for other cases we want to know what is current value. Additionally often we only want if something changed
    //example: when using rotary encoder to set termostat temperature, or sound volume etc

    //if value is changed compared to our last read
    if (encoderDelta != 0)
    {
        //now we need current value
        int16_t encoderValue = rotaryEncoder.readEncoder();
        //process new value. Here is simple output.
        currentDMXChannel = (int)encoderValue;
        Serial.print("Value: ");
        Serial.println(encoderValue);
    }
}

void encoder_setup()
{
    debug_("    Beginning Rotary Encoder...");
    rotaryEncoder.begin();
    debug("ok");
    debug_("    Loading Rotary Encoder's Setup...");
    rotaryEncoder.setup([] { rotaryEncoder.readEncoder_ISR(); });
    debug("ok");
    debug_("    Setting Rotary Encoder's Boundaries...");
    //optionally we can set boundaries and if values should cycle or not
    rotaryEncoder.setBoundaries(0, 10, true); //minValue, maxValue, cycle values (when max go to min and vice versa)
    debug("ok");

    debug_("    Enabling Rotary Encoder...");
    rotaryEncoder.enable();
    debug("ok");
}