void debug_(String message) {
#ifdef DEBUG
  Serial.print(message);
#endif
}

void debug(IPAddress ip){
#ifdef DEBUG
  Serial.println(ip);
#endif
}

void debug(String message) {
#ifdef DEBUG
  Serial.println(message);
#endif
}

void debug(const char * format, String parameters) {
#ifdef DEBUG
  Serial.printf(format, parameters);
#endif
}

void ndebug(String message) {
#ifndef DEBUG
  Serial.println(message);
#endif
}
