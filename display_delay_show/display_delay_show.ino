#include <LiquidCrystal_I2C.h>
#define ANALOG_IN_PIN A0
// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 47000.0;  //30000.0;
float R2 = 10000.0;  //7500.0;

// Float for Reference Voltage
float ref_voltage = 5.10;

// Integer for ADC value
int adc_value = 0;
//////////////////////////////////////////////////
// Initialize the LCD library with the I2C address of the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// BigNumbers bigNum(&lcd);
int current = 5;
bool dat1 = true, dat2 = true;
unsigned long previousMillis = 0, previousMillis2 = 0;
const long interval = 5000;  // 20 seconds
// LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  // Initialize the LCD
  // TCCR1B = TCCR1B & 0b11111000 | 0x01;  // use for Arduino Uno
  // TCCR2B = TCCR1B & 0b11111000 | 0x01; // use for Arduino Mega2560
  // bigNum.begin();  // set up BigNumbers
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial.println("Data Set ");
  // Print initial data
  lcd.print("Data Set 1");
  lcd.setCursor(0, 1);
  lcd.print("Data Set 2");
  // bigNum.displayLargeInt(current, 0, 4, false);
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
    lcd.clear();
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
  // if (millis() - previousMillis2 >= 30000 && millis() - previousMillis2 <= 40000) {

  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("Date : 30 Jan 2023");
  //   // Serial.println("Data Set 3");
  // }
  if (millis() - previousMillis2 >= 40000) {
    previousMillis2 = millis();
  }
  delay(200);
}

void Voltage() {
  adc_value = analogRead(ANALOG_IN_PIN);

  // Determine voltage at ADC input
  adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  in_voltage = adc_voltage * (R1 + R2) / R2;

  // Print results to Serial Monitor to 2 decimal places
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Input Voltage");
  lcd.setCursor(1, 1);
  lcd.print("  DC ");
  lcd.print(in_voltage, 2);
  lcd.print("V");
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);

  // Short delay
  delay(100);
}