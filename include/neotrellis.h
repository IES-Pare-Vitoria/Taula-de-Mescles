
#define Y_DIM 4 //number of rows of key
#define X_DIM 8 //number of columns of keys

#define NEOTRELLIS_COUNT Y_DIM * X_DIM

//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM / 4][X_DIM / 4] = {

    {Adafruit_NeoTrellis(0x31), Adafruit_NeoTrellis(0x33)}

};

//pass this matrix to the multitrellis object
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM / 4, X_DIM / 4);

//define a callback for key presses
TrellisCallback blink(keyEvent evt)
{
    if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING){
        trellis.setPixelColor(evt.bit.NUM, neotrellis_colors[evt.bit.NUM]); //on rising
        String cmd = neotrellis_commands[evt.bit.NUM];
        if(cmd.length() > 0){
            Serial.print("C-");
            Serial.println(cmd);
            mqtt_publish(MQTT_TOPIC_COMMANDS, cmd);
        }
    }else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
        trellis.setPixelColor(evt.bit.NUM, 0); //off falling

    trellis.show();
    return 0;
}

void neotrellis_setup()
{
    Serial.print("  Initializing Neotrellis...");
    if (trellis.begin())
        Serial.println("ok");
    else
    {
        Serial.println("failed to begin trellis");
        return;
    }

    for(int c = 0; c < NEOTRELLIS_COUNT; c++){
        neotrellis_colors[c] = 0xFFFFFF;
    }

    /* the array can be addressed as x,y or with the key number */
    for (int i = 0; i < Y_DIM * X_DIM; i++)
    {
        trellis.setPixelColor(i, 0xFFFFFF); //addressed with keynum
        trellis.show();
        delay(50);
    }

    for (int y = 0; y < Y_DIM; y++)
    {
        for (int x = 0; x < X_DIM; x++)
        {
            //activate rising and falling edges on all keys
            trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
            trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
            trellis.registerCallback(x, y, blink);
            trellis.setPixelColor(x, y, 0x000000); //addressed with x,y
            trellis.show();                        //show all LEDs
            delay(50);
        }
    }
}

void neotrellis_loop()
{
    trellis.read();
}