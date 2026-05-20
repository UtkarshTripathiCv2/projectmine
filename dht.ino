#include <SimpleDHT.h>

int pinDHT11 = 2; // Data pin connected to Pin 2
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(9600);
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  
  // Read the raw bytes directly from the sensor
  int err = dht11.read(&temperature, &humidity, NULL);
  
  if (err == SimpleDHTErrSuccess) {
    Serial.print((int)temperature);
    Serial.print(" C, ");
    Serial.print((int)humidity);
    Serial.println(" %");
  } else {
    Serial.println("Read failed");
  }
  
  delay(1500); // Small delay before next sample
}
