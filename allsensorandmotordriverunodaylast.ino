#include "DHT.h"

// --- Motor Driver Pins ---
#define PWMA 3
#define AIN1 4
#define AIN2 5
#define PWMB 6
#define BIN1 7
#define BIN2 8
#define STBY 9

// --- Sensor Pins ---
#define DHTPIN 2
#define DHTTYPE DHT11
#define MQ2_PIN A0
#define SOIL_PIN A1
#define RAIN_PIN A2
#define FLAME_PIN A3

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Initialize Motor Pins
  int motorPins[] = {PWMA, AIN1, AIN2, PWMB, BIN1, BIN2, STBY};
  for(int i=0; i<7; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  // Enable Driver
  digitalWrite(STBY, HIGH);
  
  Serial.println("System Initialized...");
}

void loop() {
  // 1. Read All Sensors
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gas = analogRead(MQ2_PIN);
  int soil = analogRead(SOIL_PIN);
  int rain = analogRead(RAIN_PIN);
  int flame = analogRead(FLAME_PIN);

  // 2. Check for DHT errors
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read DHT sensor!");
  } else {
    printData(t, h, gas, soil, rain, flame);
  }

  // 3. Logic & Motor Control
  // Example: If Flame detected (Typical analog < 200) or Gas high (> 500)
  if (flame < 200 || gas > 500) {
    Serial.println("ALERT: Danger Detected! Motors Running.");
    moveForward(255); // Full Speed
  } 
  else if (soil > 800) { // Soil is dry
    Serial.println("Dry Soil: Moving to water source...");
    moveForward(150); // Medium Speed
  }
  else {
    stopMotors();
  }

  delay(500); // Short delay for stability
}

// --- Motor Control Functions ---

void moveForward(int speed) {
  // Motor A
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);

  // Motor B
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speed);
}

void stopMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
}

void printData(float t, float h, int g, int s, int r, int f) {
  Serial.print("T:"); Serial.print(t);
  Serial.print("C H:"); Serial.print(h);
  Serial.print("% | G:"); Serial.print(g);
  Serial.print(" S:"); Serial.print(s);
  Serial.print(" R:"); Serial.print(r);
  Serial.print(" F:"); Serial.println(f);
}
