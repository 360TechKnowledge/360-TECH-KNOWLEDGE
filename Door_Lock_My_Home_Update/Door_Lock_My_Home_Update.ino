#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <rdm6300.h>
#include "index.h"
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#define RDM6300_RX_PIN 4  //D2
// #define READ_LED_PIN D1
#define Lock 5     //D1
#define buzzer 3   // D0
#define red 14     //D5
#define green 12   //D6
#define button 13  //D7
WiFiManager wm;
Rdm6300 rdm6300;
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif
int tagRead, sw, gpio4Value;
unsigned long MillisGreen = 0, MillisRed = 0;
bool greenStatus = false, redStatus = false, webControl = true;

// WiFiServer espServer(80);
ESP8266WebServer server(80);

// Known RFID tag UIDs to compare against
const int num_known_uids = 3;  // Number of known RFID tag UIDs
const char known_uids[num_known_uids][11] = {
  "10881746",  // Example known RFID UID 1
  "5144886",    // Example known RFID UID 2
  "02005A3FD3"  // Example known RFID UID 3
};

//////////////////OTA2024?????????

unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}



// end ////////////

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Lock, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  digitalWrite(red, 1);
  digitalWrite(green, 1);
  digitalWrite(LED_BUILTIN, HIGH);
  //  digitalWrite(blue, 1);
  delay(100);
  digitalWrite(red, 0);
  digitalWrite(green, 0);
  //  digitalWrite(blue, 0);
  WiFi.mode(WIFI_STA);
  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(60);
  if (wm.autoConnect()) {
    debugln(F("Wifi Connected!"));
    //    setRgbColor(0, 0, 0);//ALL LED LOW
  } else {
    debugln(F("Configportal running"));
    //    setRgbColor(100, 100, 100);//ALL LED HIGH
  }
  rdm6300.begin(RDM6300_RX_PIN);
  debugln("\nPlace RFID tag near the rdm6300...");
  MillisGreen = millis();
  MillisRed = millis();

  if (webControl == true) {
    server.on("/", handleRoot);
    server.on("/lock_set", lock_control);
  } else {
    server.on("/", []() {
      server.send(200, "text/plain", "Rasel Home");
    });
  }

  // espServer.begin();          /* Start the HTTP web Server */
  ElegantOTA.begin(&server);  // Start ElegantOTA
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  server.begin();
}

void loop() {
  // sw = digitalRead(button);
  // Serial.println(sw);
  // if (sw == 0) {
  //   greenled_beep(1);
  // }
  RDM();
  server.handleClient();
  ElegantOTA.loop();
  wm.process();
  millisCheck();
}

bool isUIDInArray(const char* tagRead) {
  for (int i = 0; i < num_known_uids; i++) {    // Loop through known UIDs
    if (strcmp(tagRead, known_uids[i]) == 0) {  // If the UIDs match
      return true;                              // The UID is known
    }
  }
  return false;  // If no match is found, the UID is unknown
}

void RDM() {
  if (rdm6300.get_new_tag_id()) {
    digitalWrite(LED_BUILTIN, rdm6300.get_tag_id());
    // tagRead = rdm6300.get_tag_id(), DEC;
    int tag_id = rdm6300.get_tag_id();  // Get the RFID tag ID as an integer
    tagRead = tag_id;                   // Store the tag ID
    debugln(tag_id);

    String tag_id_str = String(tag_id);  // Convert the tag ID to a string

    if (isUIDInArray(tag_id_str.c_str())) {  // Check if the UID is in the array
      Serial.println("Access Granted!");     // If known
      buzzer_beep(1);
      greenled_beep(1);
    } else {
      Serial.println("Access Denied.");  // If unknown
      buzzer_beep(3);
      redled_beep(3);
    }
    // if (tagRead == 10881746 || tagRead == 1206334) {
    //   buzzer_beep(1);
    //   greenled_beep(1);
    // } else if (tagRead != (10879861) || tagRead != (13159803) || tagRead != (1206334)) {
    //   buzzer_beep(3);
    //   redled_beep(3);
    // }
  }
}
void buzzer_beep(int times) {
  for (int i = 0; i <= times; i++) {
    Serial.println("buzzer on");
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
  }
}
// void redled_beep(int times) {
//   for (int i = 0; i <= times; i++) {
//     digitalWrite(red, HIGH);
//     delay(100);
//     digitalWrite(red, LOW);
//   }
// }
void redled_beep(int times) {
  Serial.println(redStatus);
  if (redStatus == 0)
    for (int i = 0; i <= times; i++) {
      Serial.println("red led blinking");
      digitalWrite(red, HIGH);
      redStatus = true;
      Serial.println(redStatus);
      MillisRed = millis();
    }
}
void greenled_beep(int times) {
  for (int i = 0; i <= times; i++) {
    // Serial.println("green led blinking");
    digitalWrite(green, HIGH);
    digitalWrite(Lock, HIGH);
    greenStatus = true;
    MillisGreen = millis();
  }
}
void millisCheck() {
  if (greenStatus)
    if (millis() - MillisGreen >= 2000) {
      Serial.println("green led off");
      digitalWrite(green, LOW);
      digitalWrite(Lock, LOW);
      greenStatus = false;
    }
  if (redStatus)
    if (millis() - MillisRed >= 100) {
      Serial.println("red led off");
      digitalWrite(red, LOW);
      redStatus = false;
    }
}

void handleRoot() {
  String s = webpage;
  server.send(200, "text/html", s);
}

void lock_control() {
  String state = "OFF";
  String act_state = server.arg("state");
  if (act_state == "1") {
    //    digitalWrite(LED, HIGH); //LED ON
    // user_allow_status();
    // Serial.println("Lock on");
    buzzer_beep(1);
    greenled_beep(1);
    state = "ON";
  } else {
    //    digitalWrite(LED, LOW); //LED OFF
    state = "OFF";
  }
  server.send(200, "text/plane", state);
}