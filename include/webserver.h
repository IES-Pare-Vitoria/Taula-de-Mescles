
#define HTTP_BAD_REQUEST 400
#define HTTP_OK 200

AsyncWebServer server(80);

// TODO: Move this somewhere
bool is_numeric(String number)
{
    for (int c = 0; c < number.length(); c++)
        if (!isDigit(number.charAt(c)))
            return false;

    return true;
}

void ws_send(int code, String msg, AsyncWebServerRequest *request)
{
    int l = msg.length() + 1;
    char data[l];
    msg.toCharArray(data, l);

    AsyncWebServerResponse *response = request->beginResponse(code, "text/plain", msg);
    response->addHeader("Access-Control-Max-Age", "10000");
    response->addHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void ws_bl_listener(AsyncWebServerRequest *request) // Specific Button Listener
{
    bool hasId = request->hasParam("id");
    if (!hasId)
    {
        String msg = "{ \"cbc_error\":[ \"no_id\" ] }";
        ws_send(HTTP_OK, msg, request);
        return;
    }

    preferences.begin(PREF_KEY, false);

    String buttonIdRaw = request->getParam("id")->value();

    bool buttonNumeric = is_numeric(buttonIdRaw);
    if (!buttonNumeric)
    {
        String msg = "{ \"cbc_error\":[ \"button_no_number\" ] }";
        ws_send(HTTP_OK, msg, request);
        return;
    }

    int buttonId = buttonIdRaw.toInt();
    String message = "{ \"button_data\": { \"index\":\"" + String(buttonId) + "\", \"color\":\"0x" + String(neotrellis_colors[buttonId], HEX) + "\", \"commands\":[";
    for (int c = 0; c < commandsCount; c++)
    {
      int commandButtonId = neotrellis_commands[c][0].toInt();

      if (buttonId != commandButtonId)
        continue;

      String commandButtonCmd = neotrellis_commands[c][1];
      message += "\"" + commandButtonCmd + "\",";
    }
    message = message.substring(0, message.length() - 1);
    message += "]}}";
    Serial.println("Asked for listeners of button #" + String(buttonId) + ": " + message);

    ws_send(HTTP_OK, message, request);
}

void ws_al_listener(AsyncWebServerRequest *request)
{
    // TODO: Clean up this shit
    String msgPrefix = "{\"buttons\":[";
    String message = msgPrefix;
    for (int buttonId = 0; buttonId < NEOTRELLIS_COUNT; buttonId++)
    {
        String buttonMsg = "{\"button_data\":{\"color\":\"" + String(neotrellis_colors[buttonId], HEX) + "\",\"commands\":[";
        int thisButtonCommandCounter = 0;
        for (int c = 0; c < commandsCount; c++)
        {
            int commandButtonId = neotrellis_commands[c][0].toInt();

            if (buttonId != commandButtonId)
                continue;

            String commandButtonCmd = neotrellis_commands[c][1];
            buttonMsg += "\"" + commandButtonCmd + "\",";
            thisButtonCommandCounter++;
        }
        if (thisButtonCommandCounter > 0)
            buttonMsg = buttonMsg.substring(0, buttonMsg.length() - 1);
        buttonMsg += "]}},";
        message += buttonMsg;
    }
    message = message.substring(0, message.length() - 1);
    message += "]}";

    ws_send(HTTP_OK, message, request);
}

void ws_cbc_listener(AsyncWebServerRequest *request) // Config Button Color
{
    bool hasId = request->hasParam("id");
    bool hasColor = request->hasParam("color");
    if (!hasId || !hasColor)
    {
        String err = "[ ";
        if (!hasColor)
            err += "\"no_color\",";
        if (!hasId)
            err += "\"no_id\",";
        err = err.substring(0, err.length() - 1);
        err += "]";

        String msg = "{ \"cbc_error\":" + err + " }";
        ws_send(HTTP_OK, msg, request);
        return;
    }

    preferences.begin(PREF_KEY, false);

    String buttonIdRaw = request->getParam("id")->value();
    String colorRaw = request->getParam("color")->value();

    bool buttonNumeric = is_numeric(buttonIdRaw);
    bool colorNumeric = is_numeric(colorRaw);
    if (!buttonNumeric || !colorNumeric)
    {
        String err = "[ ";
        if (!buttonNumeric)
            err += "\"button_no_number\",";
        if (!colorNumeric)
            err += "\"color_no_number\",";
        err = err.substring(0, err.length() - 1);
        err += "]";

        String msg = "{ \"cbc_error\":" + err + " }";
        ws_send(HTTP_OK, msg, request);
        return;
    }

    int buttonId = buttonIdRaw.toInt();
    int color = colorRaw.toInt();
    neotrellis_colors[buttonId] = color;

    String prefKeyStr = "btnColor" + buttonIdRaw;
    char prefKey[prefKeyStr.length() + 1];
    prefKeyStr.toCharArray(prefKey, prefKeyStr.length() + 1);

    preferences.putInt(prefKey, color);
    Serial.println("Changing color of button #" + buttonIdRaw + " to " + String(color, HEX));

    preferences.end();

    ws_send(HTTP_OK, "{ \"result\":\"ok\" }", request);
}

void ws_cbal_listener(AsyncWebServerRequest *request) // Config Button Add Listener
{
    bool hasId = request->hasParam("id");
    bool hasCommand = request->hasParam("command");
    if (!hasId || !hasCommand)
    {
        String err = "[ ";
        if (!hasCommand)
            err += "\"no_command\",";
        if (!hasId)
            err += "\"no_id\",";
        err = err.substring(0, err.length() - 1);
        err += "]";

        String msg = "{ \"cbc_error\":" + err + " }";
        ws_send(HTTP_OK, msg, request);
        return;
    }

    preferences.begin(PREF_KEY, false);

    String buttonIdRaw = request->getParam("id")->value();
    String command = request->getParam("command")->value();

    bool buttonNumeric = is_numeric(buttonIdRaw);
    if (!buttonNumeric)
    {
        String msg = "{ \"cbc_error\":[\"button_no_number\"] }";
        ws_send(HTTP_OK, msg, request);
        return;
    }

    int buttonId = buttonIdRaw.toInt();
    neotrellis_commands[commandsCount][0] = buttonIdRaw;
    neotrellis_commands[commandsCount][1] = command;

    Serial.println("Adding new listener for button #" + buttonIdRaw + " with command: \"" + command + "\"");
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
    preferences.putString(buttonCommandPrefKey, command);
    preferences.putInt(buttonIdPrefKey, buttonId);
    preferences.putInt(PREF_COMMAND_COUNT, commandsCount + 1);
    /* END STORING */

    commandsCount++;

    debug("  Closing preferences...");
    preferences.end();

    ws_send(HTTP_OK, "{ \"result\":\"ok\" }", request);
}

void ws_setup()
{
    Serial.print("  Setting up Web Server...");
    debugln();

    server.on("/get/all_listeners", HTTP_GET, ws_al_listener);
    server.on("/config/button/color", HTTP_GET, ws_cbc_listener);
    server.on("/config/button/command", HTTP_GET, ws_cbal_listener);

    server.begin();

    ndebug("ok");
}