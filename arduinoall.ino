#include <DHT.h>

// --- Pin Definitions ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ_PIN A0
#define RAIN_PIN A1
#define SOIL_PIN A2
#define FLAME_PIN A3

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("--- Arduino Sensor Hub: Health Check ---");
  dht.begin();
  
  // Initialize Analog Pins
  pinMode(MQ_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
}

void loop() {
  // 1. Read Digital/Bus Sensors
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // 2. Read Analog Sensors
  int gas = analogRead(MQ_PIN);
  int rain = analogRead(RAIN_PIN);
  int soil = analogRead(SOIL_PIN);
  int flame = analogRead(FLAME_PIN);

  // 3. Print Results to Serial Monitor
  Serial.println("================================");
  
  // Temperature & Humidity Check
  if (isnan(t) || isnan(h)) {
    Serial.println("DHT11: ERROR (Check wires!)");
  } else {
    Serial.print("Temp: "); Serial.print(t); Serial.print(" C | ");
    Serial.print("Hum: "); Serial.print(h); Serial.println(" %");
  }

  // Gas Sensor Check
  Serial.print("Gas (MQ2): "); Serial.println(gas);
  
  // Rain Sensor (Usually ~1023 is dry, lower is wet)
  Serial.print("Rain Level: "); Serial.println(rain);
  
  // Soil Moisture (Usually ~1023 is dry, lower is wet)
  Serial.print("Soil Moisture: "); Serial.println(soil);
  
  // Flame Sensor (Usually ~1023 is no fire, lower is fire)
  Serial.print("Flame Sensor: "); Serial.println(flame);

  Serial.println("================================\n");

  delay(2000); // Wait 2 seconds for next reading
}
