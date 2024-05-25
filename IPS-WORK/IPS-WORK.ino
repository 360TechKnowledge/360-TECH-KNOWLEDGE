/*-------Author by--------- 
------Md. Rasel Ahmed------*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>


const char* ssid = "RASEL HOME";
const char* password = "Home@2004";

ESP8266WebServer server(80);

#define ESP8266

#ifdef ESP8266
#include <ESP8266WiFi.h>
#define ANALOG_IN_PIN A0
#define RELAY_PIN D4            // GPIO2 on Wemos D1 Mini
const float REF_VOLTAGE = 3.3;  // ESP8266 ADC reference voltage is 3.3V

#elif defined(ARDUINO_AVR_NANO)
#define ANALOG_IN_PIN A0
#define RELAY_PIN 4
const float REF_VOLTAGE = 5.0;  // Arduino Nano reference voltage is 5V

#else
#error "Unsupported board selected. Please use an ESP8266 or Arduino Nano."
#endif

// Voltage thresholds
const float FULL_CUTOFF_VOLTAGE = 13.7;
const float LOW_CUTOFF_VOLTAGE = 13.0;

// Voltage measurement variables
float adc_voltage = 0.0;
float in_voltage = 0.0;
float averageVoltage;

// Resistor values for voltage divider (in ohms)
const float R1 = 46000.0;  // 47KOhm      45.8k
const float R2 = 9000.0;  // 10KOhm      9.8k

// ADC value
int adc_value = 0;

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timing variables
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 5000;  // 5 seconds

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Hello, this is the root page!");
  });

  // Initialize ElegantOTA
  ElegantOTA.begin(&server);

  server.begin();
  Serial.println("HTTP server started");

  Serial.println("System Initialized");
  lcd.print("System Initialized");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  delay(2000);
}

void loop() {
  server.handleClient();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis2 <= 5000) {
    displayWelcomeMessage();
  } else if (currentMillis - previousMillis2 <= 15000) {
    Voltage();
  } else if (currentMillis - previousMillis2 <= 30000) {
    displayVoltageStatus();
  } else if (currentMillis - previousMillis2 >= 40000) {
    previousMillis2 = currentMillis;
  }

  delay(500);
}

void displayWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("360 TECH KNOWLEDGE");
  lcd.setCursor(0, 1);
  lcd.print("DSP Solar IPS");
}

void Voltage() {
  const int numReadings = 50;
  float voltageReadings[numReadings];
  float totalVoltage = 0.0;

  for (int i = 0; i < numReadings; i++) {
    adc_value = analogRead(ANALOG_IN_PIN);
    adc_voltage = (adc_value * REF_VOLTAGE) / 1023.0;
    in_voltage = adc_voltage * (R1 + R2) / R2;
    voltageReadings[i] = in_voltage;
    totalVoltage += voltageReadings[i];
    delay(50);
  }

  averageVoltage = totalVoltage / numReadings;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Input Voltage");
  lcd.setCursor(1, 1);
  lcd.print("DC ");
  lcd.print(averageVoltage, 2);
  lcd.print("V");

  if (averageVoltage < LOW_CUTOFF_VOLTAGE) {
    Serial.println("Low voltage detected. Charging On.");
    digitalWrite(RELAY_PIN, HIGH);
  } else if (averageVoltage > FULL_CUTOFF_VOLTAGE) {
    Serial.println("Full voltage detected. Charging off.");
    digitalWrite(RELAY_PIN, LOW);
  }
}

void displayVoltageStatus() {
  if (in_voltage >= 14.2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Battery Fully Charged");
    lcd.setCursor(1, 1);
    lcd.print("DC ");
    lcd.print(in_voltage, 2);
    lcd.print("V");
  } else if (in_voltage >= 13) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Standard Charged Full");
    lcd.setCursor(1, 1);
    lcd.print("DC ");
    lcd.print(in_voltage, 2);
    lcd.print("V");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Battery LOW!!");
    lcd.setCursor(0, 1);
    lcd.print("Connect Charger");
  }
}
