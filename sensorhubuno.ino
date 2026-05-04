#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ_PIN A0
#define RAIN_PIN A1
#define SOIL_PIN A2
#define FLAME_PIN A3

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Pin 1 is TX. We use 9600 baud for stable communication.
  Serial.begin(9600); 
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int g = analogRead(MQ_PIN);
  int r = analogRead(RAIN_PIN);
  int s = analogRead(SOIL_PIN);
  int f = analogRead(FLAME_PIN);

  // Formatting: Temp,Hum,Gas,Rain,Soil,Flame
  // We send one clean line every 2 seconds
  Serial.print(t); Serial.print(",");
  Serial.print(h); Serial.print(",");
  Serial.print(g); Serial.print(",");
  Serial.print(r); Serial.print(",");
  Serial.print(s); Serial.print(",");
  Serial.println(f); 

  delay(2000); 
}
