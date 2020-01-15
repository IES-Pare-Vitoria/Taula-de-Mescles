#define PREF_KEY "my-app"

#define PREF_KEY_TYPE_UNKNOWN "n/a"
#define PREF_KEY_TYPE_STRING "str"
#define PREF_KEY_TYPE_INTEGER "int"

#define PREF_ANALOG_READ_RESOLUTION "anReadRes"
#define PREF_ANALOG_READ_SAMPLES "anReadSam"

#define PREF_COMMAND_COUNT "btnCommandCount"

#define PREF_DISPLAY_BLINK_ON "dispBlnkOn"
#define PREF_DISPLAY_BLINK_OFF "dispBlnkOff"

Preferences preferences;

#define validKeysCount 4
String validKeys[validKeysCount][2] = {
  { PREF_ANALOG_READ_RESOLUTION, PREF_KEY_TYPE_INTEGER }, 
  { PREF_ANALOG_READ_SAMPLES, PREF_KEY_TYPE_INTEGER }, 
  { PREF_DISPLAY_BLINK_ON, PREF_KEY_TYPE_INTEGER }, 
  { PREF_DISPLAY_BLINK_OFF, PREF_KEY_TYPE_INTEGER }
};

bool isValidKey(String key) {
  for (int c = 0; c < validKeysCount; c++)
    if (validKeys[c][0] == key)
      return true;
  return false;
}
String getKeyType(String key) {
  for (int c = 0; c < validKeysCount; c++)
    if (validKeys[c][0] == key)
      return validKeys[c][1];

  return PREF_KEY_TYPE_UNKNOWN;
}

int readPref(const char* key, int32_t defaultValue) {
  return preferences.getInt(key, defaultValue);
}
