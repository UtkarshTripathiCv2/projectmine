#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Set up the GPS serial port
SoftwareSerial gpsSerial(4, 3); // RX=4, TX=3

// Create the GPS object
TinyGPSPlus gps;

// Timer variable to show it is still searching
unsigned long lastSearchTime = 0;

void setup() {
  // Start Serial Monitor at 115200
  Serial.begin(115200);
  
  // Start GPS module at 9600
  gpsSerial.begin(9600); 

  Serial.println("=========================================");
  Serial.println("      GPS Continuous Tracker Started     ");
  Serial.println("=========================================");
  Serial.print("Searching the sky for satellites");
}

void loop() {
  // 1. Constantly feed data from the module to the TinyGPS++ object
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 2. If a valid location is found AND it is a fresh update, print it!
  if (gps.location.isUpdated()) {
    Serial.println(); // Go to a new line
    Serial.print("[LOCK ACQUIRED!] Lat: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" | Lng: ");
    Serial.print(gps.location.lng(), 6);
    Serial.print(" | Satellites: ");
    Serial.println(gps.satellites.value());
  } 
  
  // 3. If no fix yet, print a dot every 2 seconds so you know it hasn't frozen
  else if (!gps.location.isValid()) {
    if (millis() - lastSearchTime > 2000) {
      Serial.print(".");
      lastSearchTime = millis();
    }
  }
}
