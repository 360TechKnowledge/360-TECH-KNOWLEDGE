#include <avr/sleep.h>

// #define relayPin 0 // Pin 0 for controlling the relay

unsigned long startTime;
// unsigned long delayDuration = 7L * 60L * 60L * 1000L;  // 7 hours in milliseconds
unsigned long delayDuration = 5UL * 60UL * 60UL * 1000UL;  //5h delay
void setup() {
  // pinMode(relayPin, OUTPUT);
  DDRB |= (1 << DDB0);  //PB5 = O/P
  startTime = millis();
  //    Serial.begin(9600);
  //    Serial.println(delayDuration);
  PORTB |= (1 << PB0);  //PB5 = HIGH
  delay(3000);
  PORTB &= ~(1 << PB0);  //PB5 = LOW
  delay(2000);
}

void loop() {
  // Turn on the relay for 7 hours
  // digitalWrite(relayPin, 0);
  //    Serial.println(millis());
  if (millis() - startTime >= delayDuration) {
    // digitalWrite(relayPin, 1);
    PORTB |= (1 << PB0);  //PB5 = HIGH
    delay(500);
    // Put the ATtiny13 to sleep to conserve power
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
  }
}
