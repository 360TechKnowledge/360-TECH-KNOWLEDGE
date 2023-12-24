#include "time.h"  // ESP RTC
#include <Wire.h>
#include <Arduino.h>
#include <rdm6300.h>
// #include <TimeLib.h>
#include <WiFiUdp.h>
#include <ElegantOTA.h>  // OTA Library
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
////////////////////ESP RTC HEADER START/////////////////////////////////
const char* ntpServer = "asia.pool.ntp.org";
const long gmtOffset_sec = 6 * 60 * 60;  //Replace with your GMT offset (seconds)
const int daylightOffset_sec = 0;        //Replace with your daylight offset (seconds)
time_t now;                              // this is the epoch
tm tm;                                   // the structure tm holds time information in a more convient way

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds
////////////////////ESP RTC HEADER END/////////////////////////////////
String apiKeyValue = "tPmAT5Ab3j7F9";

WiFiManager wm;
Rdm6300 rdm6300;

WiFiServer TelnetServer(23);
WiFiClient Telnet;

ESP8266WebServer server(80);

#define DEBUG 1
#if DEBUG == 1
//#define debug(x) TelnetServer.print(x)
//#define debugln(x) TelnetServer.println(x)
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

//const unsigned long interval = 12UL*60UL*60UL*1000UL; //TWELVE_HRS
#define TWELVE_HRS 120000UL  //Every Minute
#define b_led 16             //D0 Blue
// #define 5                 //D1 Pin SCL
// #define 4                 //D2 Pin SDA
#define RDM6300_RX_PIN 14  //D5 RDM
#define buzzer 12          //D6 Buzzer
#define r_led 13           //D7 RED
#define g_led 15           //D8 Green

int count = 0, employee_id, employee_db_id;
bool loadOn = false, timeShow = false, lockOn, rejectOn, redLedOn, espRebootTimeOn, card_status = true, solinoidLock = true, commonGround = false, webControl = true, noResponse, netSign;
String data, dd = "", Chipid = "", payload = "", prev_data = "", read_data = "", card_number[1000], infoReadings, uname;
unsigned long loadOnTime, timeShowStart, startTime, ledOnTime, lockOnTime, rejectOnTime, espRebootTime, lastTime3 = 0, rebootOnTime, noResponseTime;
const char* serverName6 = "http://192.168.0.142/device_log.php";      //Store device info in DB
const char* serverName7 = "http://192.168.0.142/Attendance_log.php";  //Store device info in DB
const char* serverName = "http://erp.superhomebd.com/member_access_api/employee_attendance.php?access-token=ddfecef614aede69ed8ce3f4cc3a0931";
// const char* serverName = "http://erp.superhomebd.com/member_access_api/employee_attendance.php?access-token=ddfecef614aede69ed8ce3f4cc3a0931&check-environment=true&card-number=0013106718";

const char* ota_id = "neways";             //OTA USER
const char* ota_password = "neways12345";  //OTA PASSWARD

LiquidCrystal_I2C lcd(0x27, 16, 2);
/////////////Network Sign//////////////////////
// Make custom characters:
byte Check[] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};
byte Skull[] = {
  B00000,
  B01110,
  B10101,
  B11011,
  B01110,
  B01110,
  B00000,
  B00000
};
/////////////Network Sign//////////////////////
void setup() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(0, Skull);
  lcd.createChar(1, Check);
  // displayMessage("   Welcome To", "  Rasel Steel");
  WiFi.mode(WIFI_STA);
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(b_led, OUTPUT);

  pinMode(buzzer, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  //  Chipid =  ESP.getChipId(), DEC;
  Chipid = String(ESP.getChipId(), DEC);

  wm.setConfigPortalBlocking(false);

  if (wm.autoConnect()) {
    lcd.clear();
    debugln(F("Wifi Connected!"));
    displayMessage(WiFi.SSID(), WiFi.localIP().toString());
    netSign = true;
    delay(1000);
    setRgbColor(0, 0, 0);  //ALL LED LOW
  } else {
    lcd.clear();
    debugln(F("Configportal running"));
    displayMessage(WiFi.hostname().c_str(), String(ESP.getFlashChipId()));
    netSign = false;
    delay(500);
    setRgbColor(100, 100, 100);  //ALL LED HIGH
  }
  rdm6300.begin(RDM6300_RX_PIN);

  Serial.print(F(__FILE__));  // Always display sketch name and date info
  Serial.print(F("\t |"));
  Serial.print(F(__DATE__));
  Serial.print(F("\t |"));
  Serial.println(F(__TIME__));
  Serial.print(F("ChipId: "));
  Serial.println(Chipid);
  Serial.printf("ChipID: %08X\n", ESP.getFlashChipId());

  lockOn = false;
  rejectOn = false;
  redLedOn = false;
  setRgbColor(0, 0, 100);  //Blue LED HIGH

  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  Serial.println(F("Place RFID tag near the rdm6300..."));
  startTime = millis();


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  server.on("/", []() {
    server.send(200, "text/plain", "Welcome to   Rasel Steel & Engineering Works");
  });

  ElegantOTA.begin(&server, ota_id, ota_password);
  server.begin();

  device_log(WiFi.SSID(), WiFi.psk(), WiFi.localIP().toString(), ota_id, ota_password);

  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
  startMillis = millis();
}

void loop() {
  wm.process();
  rdm6300_code();
  millis_check();
  handleTelnet();
  server.handleClient();
  displayTime();
  // loadControl();
  // if (timeShow = false) {
  //   printLocalTime();
  //   timeShow = true;
  // }
}
void displayTime() {
  if (loadOn == false) {
    currentMillis = millis();                   //get the current "time" (actually the number of milliseconds since the program started)
    if (currentMillis - startMillis >= period)  //test whether the period has elapsed
    {
      printLocalTime();
      startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
    }
  }
}
void loadControl() {
  // lcd.clear();
  // printLocalTime();
  // Serial.print(tm.tm_hour);
  // displayMessage(" Hour : " + String(tm.tm_hour), "Min :" + String(tm.tm_min));
  // delay(100);
  if (loadOn == false && timeShow == false)
    if (tm.tm_hour == 7 && tm.tm_min >= 32) {
      loadOn = true;
      lcd.clear();
      displayMessage("Load On!", "All Auto charging");
      loadOnTime = millis();
    }
}

void rdm6300_code() {
  if (rdm6300.get_new_tag_id()) {
    //    digitalWrite(LED_BUILTIN, LOW);
    WRITE_PERI_REG(0x60000304, 1 << LED_BUILTIN);  //LOW
    String lengthCheck = String(rdm6300.get_tag_id(), DEC);
    readDataLength(lengthCheck.length());
    debugln(read_data);
    if (((millis() - lastTime3) > 100) || (read_data != prev_data)) {
      prev_data = read_data;
      card_status = true;
      if (card_status == true) {  //didnt use in original code I added
        // jsonParsing();
        attendance_log();
      }
      lastTime3 = millis();
      //   if (card_status == true) {
      //     debugln(F("User Not found in Local DB"));
      //     debugln(F("POST request to Main Server"));
      //   }
    }
    WRITE_PERI_REG(0x60000308, 1 << LED_BUILTIN);  //HIGH
  }
}
//////////////All Millis Checking Start///////////////////
void millis_check() {
  if (loadOn)
    if (millis() - loadOnTime > 20000) {
      lcd.clear();
      // printLocalTime();
      loadOn = false;
      displayTime();
    }
  if (timeShow)
    if (millis() - timeShowStart > 15000) {
      lcd.clear();
      loadControl();
      timeShow = false;
    }
  if (lockOn)
    if (millis() - lockOnTime > 1000) {
      if (solinoidLock == true) {
        //        digitalWrite(lock, LOW);
        // WRITE_PERI_REG(0x60000308, 1 << lock);  //HIGH
      } else {
        //        digitalWrite(lock, HIGH);
        // WRITE_PERI_REG(0x60000304, 1 << lock);  //LOW
      }
      setRgbColor(100, 0, 0);  //RED LED HIGH
      lockOn = false;
      redLedOn = true;
      ledOnTime = millis();
    }

  if (redLedOn)
    if (millis() - ledOnTime > 1000) {
      setRgbColor(0, 0, 100);  //BLUE LED HIGH
      redLedOn = false;
    }

  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - rebootOnTime > 60000) {
      debugln(F("Device auto restarting"));
      ESP.restart();
      //      ESP.reset() ;
    }
  }
}
//////////////All Millis Checking End///////////////////

void user_allow_status() {
  if (solinoidLock == true) {
    //    digitalWrite(lock, HIGH);
    // WRITE_PERI_REG(0x60000304, 1 << lock);  //LOW
  } else {
    //    digitalWrite(lock, LOW);
    // WRITE_PERI_REG(0x60000308, 1 << lock);  //HIGH
  }
  setRgbColor(0, 100, 0);  //Green LED HIGH
  lockOn = true;
  lockOnTime = millis();
}

void user_deny_status() {
  setRgbColor(100, 0, 0);  //REG LED HIGH
  redLedOn = true;
  //  buzzer_beep(3);
  ledOnTime = millis();
}

//RGB led control
void setRgbColor(int r, int g, int b) {
  //if Common ground true HIGH(R=0, G=0, B=0) && LOW(R=255, G=255, B=255)
  //if Common vcc true HIGH(R=255, G=255, B=255) && LOW(R=0, G=0, B=0)
  if (commonGround == true) {
    int R = map(r, 0, 100, 0, 255);
    int G = map(g, 0, 100, 0, 255);
    int B = map(b, 0, 100, 0, 255);
    analogWrite(r_led, R);
    analogWrite(g_led, G);
    analogWrite(b_led, B);
  } else {
    int R = map(r, 0, 100, 255, 0);
    int G = map(g, 0, 100, 255, 0);
    int B = map(b, 0, 100, 255, 0);
    analogWrite(r_led, R);
    analogWrite(g_led, G);
    analogWrite(b_led, B);
  }
}

//Adding prefix 0 on card number
void readDataLength(int dataLength) {
  if (dataLength == 2) {
    read_data = "00000000";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 3) {
    read_data = "0000000";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 4) {
    read_data = "000000";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 5) {
    read_data = "00000";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 6) {
    read_data = "0000";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 7) {
    read_data = "000";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 8) {
    read_data = "00";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 9) {
    read_data = "0";
    read_data += String(rdm6300.get_tag_id(), DEC);
  } else if (dataLength == 10) {
    read_data = "";
    read_data += String(rdm6300.get_tag_id(), DEC);
  }
}

void buzzer_beep(int times, int delays) {
  for (int i = 0; i <= times; i++) {
    //    digitalWrite(buzzer, HIGH);
    WRITE_PERI_REG(0x60000304, 1 << buzzer);  //HIGH
    delay(delays);
    //    digitalWrite(buzzer, LOW);
    WRITE_PERI_REG(0x60000308, 1 << buzzer);  //LOW
    delay(delays);
  }
}

void routerCheck() {
  switch (WiFi.status()) {
    case WL_NO_SSID_AVAIL:
      debugln(F("SSID Unavailable"));
      netSign = false;
      break;
    case WL_CONNECTED:
      debugln(F("Wifi Connected"));
      netSign = true;
      break;
    case WL_CONNECT_FAILED:
      debugln(F("Connection Failed"));
      netSign = false;
      break;
  }
}

void handleTelnet() {
  if (TelnetServer.hasClient()) {
    if (!Telnet || !Telnet.connected()) {
      if (Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
      Telnet.flush();
    } else {
      TelnetServer.available().stop();
    }
  }
}

void displayMessage(String firstLine, String secondLine) {
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  networkSign();
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  String perm = "&check-environment=true&card-number=";
  // String serverPath = serverName;
  // &check-environment=true&card-number=0013106718
  String serverPath = serverName + perm + read_data;

  http.begin(client, serverPath.c_str());
  // http.begin(client, serverName);

  int httpResponseCode = http.GET();
  String payload = "{}";
  if (httpResponseCode > 0) {
    debug(F("HTTP Response code: "));
    debugln(httpResponseCode);
    payload = http.getString();
  } else {
    debug("Error code: ");
    debugln(httpResponseCode);
  }
  http.end();
  return payload;
}

void jsonParsing() {
  if (WiFi.status() == WL_CONNECTED) {
    StaticJsonDocument<200> value;

    infoReadings = httpGETRequest(serverName);
    debugln(infoReadings);
    DeserializationError error = deserializeJson(value, infoReadings);
    if (error) {
      debugln(F("deserializeJson() failed: "));
      debugln(error.f_str());
      return;
    }

    employee_id = value["employee_id"];
    employee_db_id = value["employee_db_id"];
    String names = value["name"];
    uname = names;
    String empId = String(employee_id);
    lcd.clear();
    if (names != "null") {
      displayMessage(names, "ID: " + empId);
      attendance_log();
      buzzer_beep(2, 100);
    } else {
      displayMessage("Enrolement False", "Contact Ur Boss");
      WRITE_PERI_REG(0x60000304, 1 << buzzer);  //HIGH
      delay(1000);
      WRITE_PERI_REG(0x60000308, 1 << buzzer);  //LOW
    }
    delay(1000);
    lcd.clear();
    httpGETRequest1();
  }
}

void httpGETRequest1() {
  WiFiClient client;
  HTTPClient http;
  String perm = "&submit-information=true&employee-db-id=";
  String serverPath = serverName + perm + employee_db_id;
  // &submit-information=true&employee-db-id=10434
  // String serverPath = serverName + "&submit-information=true&employee-db-id=" + employee_db_id;

  http.begin(client, serverPath.c_str());
  // http.begin(client, serverName);

  int httpResponseCode = http.GET();
  // String payload = "{}";
  String payload = http.getString();
  if (httpResponseCode > 0) {
    debug(F("HTTP Response code: "));
    debugln(httpResponseCode);
    // payload = http.getString();
    payload.trim();
    if (payload != "") {
      displayMessage(" Emp Enrolement", payload);
      delay(1000);
      lcd.clear();
    } else {
      displayMessage(" Emp Enrolement", "Unavailable");
      delay(1000);
      lcd.clear();
    }
  } else {
    debug(F("Error code: "));
    debugln(httpResponseCode);
    // buzzer_beep(2);
  }
  http.end();
  // return payload;
}

void device_log(String router_ssid, String router_pass, String device_ip, String ota_id, String ota_password) {
  if (WiFi.status() == WL_CONNECTED) {
    debugln(F("Device log"));
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName6);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKeyValue + "&device_id=" + Chipid + "&router_ssid=" + router_ssid + "&router_password=" + router_pass + "&device_ip=" + device_ip + "&ota_id=" + ota_id + "&ota_password=" + ota_password + "";
    int httpResponseCode = http.POST(httpRequestData);
    //    String payload = http.getString();
    http.end();
  }
}
/////////////////ESP RTC Function Start////////////////////
void printLocalTime() {  //ntp
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  showTime();
  timeShowStart = millis();
  timeShow = true;
}

void showTime() {  //ntp
  time(&now);      // read the current time
  localtime_r(&now, &tm);
  if (String(tm.tm_year + 1900) != "1970") {
    displayMessage("  Rasel Steel", String(tm.tm_mday) + "." + String(tm.tm_mon + 1) + "." + String(tm.tm_year + 1900).substring(2) + " " + String(tm.tm_hour) + ":" + String(tm.tm_min) + ":" + String(tm.tm_sec) + " ");  // update the structure tm with the current time
  } else {
    displayMessage("  Rasel Steel", "NTP Unavailable!");  // update the structure tm with the current time
  }
  // Serial.print(String(tm.tm_mday) + "." + String(tm.tm_mon + 1) + "." + String(tm.tm_year + 1900)); // years since 1900
  // Serial.print(" ");
  // Serial.print(String(tm.tm_hour) + ":" + String(tm.tm_min) + ":" + String(tm.tm_sec));  // seconds after the minute  0-61*
  // Serial.println();
}
/////////////////ESP RTC Function End////////////////////
void attendance_log() {
  if (WiFi.status() == WL_CONNECTED) {
    debugln(F("Attendance log"));
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName7);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKeyValue + "&device_id=" + Chipid + "&card_no=" + String(read_data) + "";
    int httpResponseCode = http.POST(httpRequestData);
    String payload = http.getString();
    // Serial.println(payload);
    if (payload == "done") {
      digitalWrite(buzzer, 1);
      delay(300);
      digitalWrite(buzzer, 0);
      lcd.clear();
      displayMessage("Attendence Done!", read_data);
      delay(800);
      lcd.clear();
    } else {
      lcd.clear();
      displayMessage("Attendence Faild!", "Server Down");
      delay(1000);
      lcd.clear();
    }

    http.end();
  }
}

void networkSign() {
  if (netSign == false) {
    lcd.setCursor(15, 0);
    lcd.write(0);
  } else if (netSign == true) {
    lcd.setCursor(15, 0);
    lcd.write(1);
  }
}