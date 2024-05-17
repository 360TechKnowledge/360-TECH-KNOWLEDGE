/*-------Author by--------- 
------Md. Rasel Ahmed------*/
#include <LiquidCrystal_I2C.h>
#define ANALOG_IN_PIN A0

///////////////voltage measure part.............
float adc_voltage = 0.0;
float in_voltage = 0.0;
float averageVoltage;

// Floats for resistor values in divider (in ohms)
float R1 = 47000.0;  //30000.0;
float R2 = 10000.0;  //7500.0;
float ref_voltage = 5.10; // Float for Reference Voltage
int adc_value = 0;
//////////////////////////////////////////////////

LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD library with the I2C address of the LCD
int current = 5;
bool dat1 = true, dat2 = true;
unsigned long previousMillis = 0, previousMillis2 = 0;
const long interval = 5000;  // 5 seconds

void setup() {

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial.println("Data Set ");
  // Print initial data
  lcd.print("Data Set 1");
  lcd.setCursor(0, 1);
  lcd.print("Data Set 2");

}

void loop() {
  if (millis() - previousMillis2 <= 10000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("360 TECH KNOWLEDGE");
    lcd.setCursor(0, 1);
    lcd.print("DSP Solar IPS");
    // Serial.println("Data Set 1");
  }
  if (millis() - previousMillis2 <= 20000 && millis() - previousMillis2 >= 10000) {
    // lcd.clear();
    Voltage();
    // Serial.println("Data Set 2");
  }
  if (millis() - previousMillis2 >= 20000 && millis() - previousMillis2 <= 30000) {
    if (in_voltage >= 14.2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Battery Fully Charged");
      lcd.setCursor(1, 1);
      lcd.print(" DC ");
      lcd.print(in_voltage, 2);
      lcd.print("V");
      // Serial.println("Data Set 3");
    } else if (in_voltage >= 13) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Standard Charged Full");
      lcd.setCursor(1, 1);
      lcd.print(" DC ");
      lcd.print(in_voltage, 2);
      lcd.print("V");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Battery LOW!!");
      lcd.setCursor(0, 1);
      lcd.print("Please Connect The Charger");
    }
  }

  if (millis() - previousMillis2 >= 40000) {
    previousMillis2 = millis();
  }
  delay(500);
}

void Voltage() {
  const int numReadings = 10;          // Number of readings to average
  float voltageReadings[numReadings];  // Array to store voltage readings
  int currentIndex = 0;                // Index for storing new readings
  float totalVoltage = 0.0;            // Variable to store total voltage readings

  // Take multiple readings and calculate average voltage
  for (int i = 0; i < numReadings; i++) {
    adc_value = analogRead(ANALOG_IN_PIN);
    //   // Determine voltage at ADC input
    adc_voltage = (adc_value * ref_voltage) / 1023.0;
    //   // Calculate voltage at divider input
    in_voltage = adc_voltage * (R1 + R2) / R2;
    voltageReadings[i] = in_voltage;     // Convert analog reading to voltage
    totalVoltage += voltageReadings[i];  // Add reading to total
    delay(50);                           // Delay between readings
  }
  averageVoltage = totalVoltage / numReadings;  // Calculate average voltage
  // Serial.print("Voltage : ");
  // Serial.println(averageVoltage);
  //   // Print results to Serial Monitor to 2 decimal places
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Input Voltage");
  lcd.setCursor(1, 1);
  lcd.print("  DC ");
  lcd.print(averageVoltage, 2);
  lcd.print("V");
  // delay(3);
  // // Check if average voltage falls below low cutoff threshold
  // if (averageVoltage < lowCutoffVoltage) {
  //   // Cut off power
  //   digitalWrite(relayPin, HIGH);
  //   Serial.println("Low voltage detected. Power cut off.");
  // }
  // // Check if average voltage rises above full cutoff threshold
  // else if (averageVoltage > fullCutoffVoltage) {
  //   // Restore power
  //   digitalWrite(relayPin, LOW);
  //   Serial.println("Full voltage detected. Power restored.");
  // }
}
//////////// old voltage function//////////////
// void Voltage() {
//   adc_value = analogRead(ANALOG_IN_PIN);

//   // Determine voltage at ADC input
//   adc_voltage = (adc_value * ref_voltage) / 1024.0;

//   // Calculate voltage at divider input
//   in_voltage = adc_voltage * (R1 + R2) / R2;

//   // Print results to Serial Monitor to 2 decimal places
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(" Input Voltage");
//   lcd.setCursor(1, 1);
//   lcd.print("  DC ");
//   lcd.print(in_voltage, 2);
//   lcd.print("V");
//   Serial.print("Input Voltage = ");
//   Serial.println(in_voltage, 2);

//   // Short delay
//   delay(100);
// }