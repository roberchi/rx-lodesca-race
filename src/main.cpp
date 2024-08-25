#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include "wificredential.h"


// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Define servos
Servo steerServo;
Servo speedServo;

// Define GPIO pins for the servos
const int steerPin = D1; // Change to your steering servo pin
const int speedPin = D2; // Change to your speed control servo pin

void connectToWiFi();

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);

  // Connect to Wi-Fi
  connectToWiFi();

  // Attach servos to their pins
  steerServo.attach(steerPin);
  speedServo.attach(speedPin);

  // Set initial position of servos
  steerServo.write(90);  // Neutral position for steering
  speedServo.write(0);  // Neutral position for speed

  // POST request to control servos
  server.on("/api/control", HTTP_POST, [](AsyncWebServerRequest *request){
    String response = "";

    if (request->hasParam("steer", true)) {
      String steerValue = request->getParam("steer", true)->value();
      int steerAngle = constrain(steerValue.toInt(), 0, 180); // Constrain to valid servo range
      steerServo.write(steerAngle);
      response += "Steer angle set to: " + String(steerAngle) + "\n";
    }

    if (request->hasParam("speed", true)) {
      String speedValue = request->getParam("speed", true)->value();
      int speedValueInt = constrain(speedValue.toInt(), 0, 180); // Constrain to valid servo range
      speedServo.write(speedValueInt);
      response += "Speed set to: " + String(speedValueInt) + "\n";
    }

    if (response == "") {
      response = "No valid parameters provided. Use 'steer' and/or 'speed'.";
    }

    request->send(200, "text/plain", response);
  });

  // Start server
  server.begin();
}

void loop() {
  // Check if Wi-Fi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Attempting to reconnect...");
    connectToWiFi();
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);

  // Attempt to connect for 10 seconds
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}