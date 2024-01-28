#include <LiquidCrystal_I2C.h>
#include <BigNumbers.h>

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
  unsigned long currentMillis = millis();

  // Display data set 1
  // if (currentMillis - previousMillis >= interval) {
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("Data Set 1");
  //   Serial.println("Data Set 1");
  //   dat1 = false;
  //   // Add your code to get and display data set 1 values here

  //   previousMillis = currentMillis;
  //   Serial.print(previousMillis);
  // }
  // Serial.println(currentMillis);
  // Display data set 2 after another 20 seconds
  if (millis() - previousMillis2 >= interval) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Data Set 2");
    Serial.println("Data Set 2");
    // dat2 = false;
    // Add your code to get and display data set 2 values here

    previousMillis2 = millis();
  }
  if (millis() - previousMillis2 >= interval) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Data Set 3");
    Serial.println("Data Set 2");
    dat2 = false;
    // Add your code to get and display data set 2 values here

    previousMillis2 = millis();
  }
  if (millis() - previousMillis2 >= interval) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Data Set 4");
    Serial.println("Data Set 4");
    dat2 = false;
    // Add your code to get and display data set 2 values here

    previousMillis2 = millis();
  }
}
