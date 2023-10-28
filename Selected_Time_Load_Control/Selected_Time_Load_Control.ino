#include <avr/sleep.h>

#define relayPin 0 // Pin 0 for controlling the relay

unsigned long startTime;
unsigned long delayDuration = 7L * 60L * 60L * 1000L;  // 7 hours in milliseconds
//unsigned long delayDuration =   1L * 10L * 60L * 1000L;//7hours * 1h=60min * 1min=60sec * 1sec=1000ms
void setup() {
  pinMode(relayPin, OUTPUT);
  startTime = millis();
  //    Serial.begin(9600);
  //    Serial.println(delayDuration);
}

void loop() {
  // Turn on the relay for 7 hours
  digitalWrite(relayPin, 0);
  //    Serial.println(millis());
  if (millis() - startTime >= delayDuration) {
    digitalWrite(relayPin, 1);
    // Put the ATtiny13 to sleep to conserve power
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
  }
}
