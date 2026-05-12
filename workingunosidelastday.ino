#include <SoftwareSerial.h>
#include "DHT.h"

// Using 10 and 11 to avoid conflict with USB/Serial Monitor
SoftwareSerial espSerial(10, 11); // RX, TX

// TB6612FNG Pins
const int PWMA = 3, AIN1 = 4, AIN2 = 5;
const int PWMB = 6, BIN1 = 7, BIN2 = 8;
const int STBY = 9;

// Sensors
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);    // For PC Debugging
  espSerial.begin(9600); // For ESP32 Communication
  dht.begin();

  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
  
  Serial.println("Uno System Ready");
}

void loop() {
  // Listen for ESP32 Commands
  if (espSerial.available()) {
    char cmd = espSerial.read();
    Serial.print("Command Received: "); Serial.println(cmd);
    executeCommand(cmd);
  }

  // Send Data every 1.5 seconds
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 1500) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    // Check for sensor failure
    if (isnan(t)) t = 0.0;
    if (isnan(h)) h = 0.0;

    // Format: Temp,Hum,Gas,Soil,Rain,Flame
    String data = String(t, 1) + "," + String(h, 1) + "," + 
                  String(analogRead(A0)) + "," + String(analogRead(A1)) + "," + 
                  String(analogRead(A2)) + "," + String(analogRead(A3));
    
    espSerial.println(data);
    Serial.println("Sent Data: " + data); // See it on your PC
    lastTime = millis();
  }
}

void executeCommand(char cmd) {
  int s = 220; 
  switch (cmd) {
    case 'F': move(s, s, HIGH, LOW, HIGH, LOW); break;
    case 'B': move(s, s, LOW, HIGH, LOW, HIGH); break;
    case 'L': move(s, s, LOW, HIGH, HIGH, LOW); break;
    case 'R': move(s, s, HIGH, LOW, LOW, HIGH); break;
    case 'S': move(0, 0, LOW, LOW, LOW, LOW); break;
  }
}

void move(int spA, int spB, int a1, int a2, int b1, int b2) {
  digitalWrite(AIN1, a1); digitalWrite(AIN2, a2); analogWrite(PWMA, spA);
  digitalWrite(BIN1, b1); digitalWrite(BIN2, b2); analogWrite(PWMB, spB);
}
