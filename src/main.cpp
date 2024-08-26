#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include "wificredential.h"


// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Define UDP server
WiFiUDP udp;
unsigned int udpPort = 4210;  // Port for UDP communication
char incomingPacket[255];  // Buffer for incoming packets
unsigned long lastProcessedTick = 0;  // Store the last processed tick

// Define servos
Servo steerServo;
Servo speedServo;

// Define GPIO pins for the servos
const int steerPin = D1; // Change to your steering servo pin
const int speedPin = D2; // Change to your speed control servo pin

void connectToWiFi();
void processUDPMessage();

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

  // Start the UDP server
  udp.begin(udpPort);
  Serial.printf("UDP server started at IP %s on port %d\n", WiFi.localIP().toString().c_str(), udpPort);


}

void loop() {
  // Check if Wi-Fi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Attempting to reconnect...");
    connectToWiFi();
  }

  // Process incoming UDP messages
  processUDPMessage();
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

// Process incoming UDP messages
// udp data format: tick|steer|speed
void processUDPMessage(){
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read the packet into the buffer
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    Serial.printf("Received packet: %s\n", incomingPacket);

    // Parse the received packet
    String packetStr = String(incomingPacket);
    int firstDelim = packetStr.indexOf('|');
    int secondDelim = packetStr.indexOf('|', firstDelim + 1);

    if (firstDelim != -1 && secondDelim != -1) {
      String tickStr = packetStr.substring(0, firstDelim);
      String steerStr = packetStr.substring(firstDelim + 1, secondDelim);
      String speedStr = packetStr.substring(secondDelim + 1);

      unsigned long currentTick = tickStr.toInt();
      int steerValue = steerStr.toInt();
      int speedValue = speedStr.toInt();

      // Check if the received tick is newer than the last processed tick
      if (currentTick > lastProcessedTick) {
        lastProcessedTick = currentTick;

        // Update the servos
        int steerAngle = constrain(steerValue, 0, 180); // Constrain to valid servo range
        int speedValueInt = constrain(speedValue, 0, 180); // Constrain to valid servo range
        steerServo.write(steerAngle);
        speedServo.write(speedValueInt);

        Serial.printf("Processed packet with tick: %lu, steer: %d, speed: %d\n", currentTick, steerAngle, speedValueInt);
      } else {
        Serial.printf("Skipped outdated packet with tick: %lu\n", currentTick);
      }
    }
  }
}