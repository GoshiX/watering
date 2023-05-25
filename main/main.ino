#include <SoftwareSerial.h>
SoftwareSerial mySerial(D5, D7); // RX, TX
void setup() {
  Serial.begin(115200);  // Скорость обмена данными с компьютером
  Serial.println("Start!");
  mySerial.begin(115200);  // Скорость обмена данными с GSM модулем
  mySerial.println("AT");
}

void loop() {
  if (mySerial.available())
    Serial.write(mySerial.read());
  if (Serial.available())
    mySerial.write(Serial.read());
}