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
      server.send(200, "text/plain", "Welcome to Neways International");
    });
  }

  // espServer.begin();          /* Start the HTTP web Server */
  ElegantOTA.begin(&server);  // Start ElegantOTA
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
  wm.process();
  millisCheck();
}
void RDM() {
  if (rdm6300.get_new_tag_id()) {
    digitalWrite(LED_BUILTIN, rdm6300.get_tag_id());
    tagRead = rdm6300.get_tag_id(), DEC;
    debugln(tagRead);
    if (tagRead == 10881746 || tagRead == 1206334) {
      buzzer_beep(1);
      greenled_beep(1);
    } else if (tagRead != (10879861) || tagRead != (13159803) || tagRead != (1206334)) {
      buzzer_beep(3);
      redled_beep(3);
    }
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

// void Web() {
//   WiFiClient client = espServer.available(); /* Check if a client is available */
//   if (!client) {
//     return;
//   }

//   Serial.println("New Client!!!");

//   String request = client.readStringUntil('\r'); /* Read the first line of the request from client */
//   Serial.println(request);                       /* Print the request on the Serial monitor */
//   /* The request is in the form of HTTP GET Method */
//   client.flush();

//   /* Extract the URL of the request */
//   /* We have four URLs. If IP Address is 192.168.1.6 (for example),
//    * then URLs are:
//    * 192.168.1.6/GPIO4ON and its request is GET /GPIO4ON HTTP/1.1
//    * 192.168.1.6/GPIO4OFF and its request is GET /GPIO4OFF HTTP/1.1
//    * 192.168.1.6/GPIO5ON and its request is GET /GPIO5ON HTTP/1.1
//    * 192.168.1.6/GPIO4OFF and its request is GET /GPIO5OFF HTTP/1.1
//    */
//   /* Based on the URL from the request, turn the LEDs ON or OFF */
//   if (request.indexOf("/GPIO4ON") != -1) {
//     Serial.println("GPIO4 LED is ON");
//     // digitalWrite(gpio4LEDPin, HIGH);
//     gpio4Value = HIGH;
//     digitalWrite(LED_BUILTIN, LOW);
//   }
//   if (request.indexOf("/GPIO4OFF") != -1) {
//     Serial.println("GPIO4 LED is OFF");
//     // digitalWrite(gpio4LEDPin, LOW);
//     digitalWrite(LED_BUILTIN, HIGH);
//     gpio4Value = LOW;
//   }


//   /* HTTP Response in the form of HTML Web Page */
//   client.println("HTTP/1.1 200 OK");
//   client.println("Content-Type: text/html");
//   client.println();  //  IMPORTANT
//   client.println("<!DOCTYPE HTML>");
//   client.println("<html>");
//   client.println("<head>");
//   client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
//   client.println("<link rel=\"icon\" href=\"data:,\">");
//   /* CSS Styling for Buttons and Web Page */
//   client.println("<style>");
//   client.println("html { font-family: Courier New; display: inline-block; margin: 0px auto; text-align: center;}");
//   client.println(".button {border: none; color: white; padding: 10px 20px; text-align: center;");
//   client.println("text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}");
//   client.println(".button1 {background-color: #13B3F0;}");
//   client.println(".button2 {background-color: #3342FF;}");
//   client.println("</style>");
//   client.println("</head>");

//   /* The main body of the Web Page */
//   client.println("<body>");
//   client.println("<h2>ESP8266 Web Server</h2>");

//   if (gpio4Value == LOW) {
//     client.println("<p>GPIO4 LED Status: OFF</p>");
//     client.print("<p><a href=\"/GPIO4ON\"><button class=\"button button1\">Click to turn ON</button></a></p>");
//   } else {
//     client.println("<p>GPIO4 LED Status: ON</p>");
//     client.print("<p><a href=\"/GPIO4OFF\"><button class=\"button button2\">Click to turn OFF</button></a></p>");
//   }

//   client.println("</body>");
//   client.println("</html>");
//   client.print("\n");

//   delay(1);
//   /* Close the connection */
//   client.stop();
//   Serial.println("Client disconnected");
//   Serial.print("\n");
// }