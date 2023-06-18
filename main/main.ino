void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, 0);
  delay(300);
  digitalWrite(LED_BUILTIN, 1);
  delay(300);
}