
#define Y_DIM 8 //number of rows of key
#define X_DIM 4 //number of columns of keys

#define NEOTRELLIS_COUNT Y_DIM *X_DIM

//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM / 4][X_DIM / 4] = {

    {Adafruit_NeoTrellis(0x33)},
    {Adafruit_NeoTrellis(0x31)}

};

//pass this matrix to the multitrellis object
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM / 4, X_DIM / 4);

//define a callback for key presses
TrellisCallback blink(keyEvent evt)
{
    if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    {
        trellis.setPixelColor(evt.bit.NUM, neotrellis_colors[evt.bit.NUM]); //on rising
        for (int c = 0; c < commandsCount; c++)
            if (neotrellis_commands[c][0].toInt() == evt.bit.NUM)
            {
                String cmd = neotrellis_commands[c][1];
                if (cmd.length() > 0)
                    sendCommand(cmd);
            }
    }
    else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
        trellis.setPixelColor(evt.bit.NUM, 0); //off falling

    trellis.show();
    return 0;
}

void neotrellis_setup()
{
    display_print("NEOT");

    Serial.print("  Initializing Neotrellis...");
    if (trellis.begin())
        Serial.println("ok");
    else
    {
        Serial.println("failed to begin trellis");
        return;
    }

    Serial.print("  Loading parameters from preferences...");
    debugln();
    debug("    Initializing preferences...");
    preferences.begin(PREF_KEY, false);

    int storedCommands = preferences.getInt(PREF_COMMAND_COUNT);
    if (storedCommands > 0)
    {
        debug("    Found " + String(storedCommands) + " stored command listeners:");
        for (int c = 0; c < storedCommands; c++)
        {
            debug("      Loading command listener #" + String(c));
            debug("        Converting button pref key...");
            String prefButtonKeyStr = "bC" + String(c) + "Btn";
            char buttonIdPrefKey[prefButtonKeyStr.length() + 1];
            prefButtonKeyStr.toCharArray(buttonIdPrefKey, prefButtonKeyStr.length() + 1);

            debug("        Converting command pref key...");
            String prefButtonCommandKeyStr = "bC" + String(c) + "Cmd";
            char buttonCommandPrefKey[prefButtonCommandKeyStr.length() + 1];
            prefButtonCommandKeyStr.toCharArray(buttonCommandPrefKey, prefButtonCommandKeyStr.length() + 1);

            debug("        Fetching command...");
            int buttonId = preferences.getInt(buttonIdPrefKey);
            String command = preferences.getString(buttonCommandPrefKey, "");

            debug("        Storing loaded command:");
            debug("          Button Id: " + String(buttonId));
            debug("          Button Command: \"" + String(command) + "\"");
            neotrellis_commands[commandsCount][0] = String(buttonId);
            neotrellis_commands[commandsCount][1] = command;

            commandsCount++;
        }
    }
    else
        debug("    Did not find any stored commands.");

    debug("    Searching for stored button colors...");
    for (int c = 0; c < NEOTRELLIS_COUNT; c++)
    {
        String prefKeyStr = "btnColor" + String(c);
        char prefKey[prefKeyStr.length() + 1];
        prefKeyStr.toCharArray(prefKey, prefKeyStr.length() + 1);

        int neoColor = preferences.getInt(prefKey);
        if (neoColor > 0)
        {
            debug("      Found stored color for button #" + String(c) + ": 0x" + String(neoColor, HEX));
            neotrellis_colors[c] = neoColor;
        }
    }

    debug("    Closing preferences...");
    preferences.end();
    ndebug("ok");

    for (int c = 0; c < NEOTRELLIS_COUNT; c++)
        if (neotrellis_colors[c] <= 0)
            neotrellis_colors[c] = 0xFFFFFF;

    for (int c = 0; c < NEOTRELLIS_COUNT; c++)
    {
        trellis.activateKey(c, SEESAW_KEYPAD_EDGE_RISING, true);
        trellis.activateKey(c, SEESAW_KEYPAD_EDGE_FALLING, true);
        trellis.registerCallback(c, blink);
        trellis.setPixelColor(c, neotrellis_colors[c]);
        if (c > 0)
            trellis.setPixelColor(c - 1, 0); // Turn off previous Pixel
        trellis.show();
        delay(50);
    }

    for (int c = 0; c < NEOTRELLIS_COUNT; c++)
    {
        trellis.setPixelColor(c, 0);
        trellis.show();
    }
}

void neotrellis_loop()
{
    trellis.read();
}