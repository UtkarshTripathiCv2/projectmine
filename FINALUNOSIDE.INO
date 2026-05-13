/*
 * ============================================================
 *  ARDUINO UNO SENSOR HUB
 *  Sends sensor data to ESP32 using SoftwareSerial
 * ============================================================
 */

#include <DHT.h>
#include <SoftwareSerial.h>

// ---------------- PINS ----------------
#define DHT_PIN      2
#define DHT_TYPE     DHT11

#define MQ2_PIN      A0
#define SOIL_PIN     A1
#define FLAME_PIN    A2
#define RAIN_PIN     A3

// SoftwareSerial RX,TX
SoftwareSerial espSerial(10, 11);

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastSend = 0;

void setup() {

  Serial.begin(9600);
  espSerial.begin(9600);

  dht.begin();

  Serial.println("UNO SENSOR HUB STARTED");

  delay(2000);
}

void loop() {

  if (millis() - lastSend < 1000) return;
  lastSend = millis();

  // ---------------- DHT11 ----------------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature)) temperature = -1;
  if (isnan(humidity)) humidity = -1;

  // ---------------- MQ2 ----------------
  int gasRaw = analogRead(MQ2_PIN);

  // ---------------- SOIL ----------------
  int soilRaw = analogRead(SOIL_PIN);

  int soilPct = map(soilRaw, 1023, 250, 0, 100);
  soilPct = constrain(soilPct, 0, 100);

  // ---------------- FLAME ----------------
  int flameRaw = analogRead(FLAME_PIN);

  int flamePct = map(flameRaw, 1023, 0, 0, 100);
  flamePct = constrain(flamePct, 0, 100);

  // ---------------- RAIN ----------------
  int rainRaw = analogRead(RAIN_PIN);

  int rainPct = map(rainRaw, 1023, 0, 0, 100);
  rainPct = constrain(rainPct, 0, 100);

  // ---------------- BUILD JSON ----------------
  String json = "{";
  json += "\"T\":" + String(temperature, 1);
  json += ",\"H\":" + String(humidity, 1);
  json += ",\"G\":" + String(gasRaw);
  json += ",\"F\":" + String(flamePct);
  json += ",\"R\":" + String(rainPct);
  json += ",\"S\":" + String(soilPct);
  json += "}";

  // Send to ESP32
  espSerial.println(json);

  // Debug Serial Monitor
  Serial.println(json);
}
