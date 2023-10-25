#include <avr/sleep.h>

const int relayPin = 13;  // Pin 0 for controlling the relay

unsigned long startTime;
unsigned long delayDuration = 4L * 30L * 60L * 1000L;  // 7 hours in milliseconds
//unsigned long delayDuration =   2L * 30L * 1000L;//7hours * 1h=60min * 1min=60sec * 1sec=1000ms
void setup() {
  pinMode(relayPin, OUTPUT);
  startTime = millis();
  //  Serial.begin(9600);
}

void loop() {
  // Turn on the relay for 7 hours
  digitalWrite(relayPin, HIGH);
  //  delay(7 * 3600000);  // 7 hours in milliseconds
  //  delay(5000);
  //  Serial.println(startTime);
  if (millis() - startTime >= delayDuration) {
    digitalWrite(relayPin, LOW);
    // Put the ATtiny13 to sleep to conserve power
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
  }
}
