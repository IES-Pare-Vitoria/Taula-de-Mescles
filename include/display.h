
#define DISPLAY_ADDRESS 0x70

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void display_init()
{
    alpha4.begin(DISPLAY_ADDRESS);
}

void display_print(String message)
{
    debug("Showing " + message + " in the display.");

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