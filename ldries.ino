#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- Pin Definitions ---
#define LDR_PIN A0
#define IR1_PIN 2
#define IR2_PIN 3
#define DHT_PIN 4
#define LED_PIN 5

// --- Constants & Thresholds ---
#define DHTTYPE DHT11
const int SPEED_LIMIT_KMH = 30; // Set your speed violation limit
const float DISTANCE_METERS = 0.40; // 40 cm distance between IR sensors
const int DARK_THRESHOLD = 600; // Adjust based on your LDR testing

// --- Objects & Variables ---
DHT dht(DHT_PIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 is the most common I2C address

unsigned long lastMotionTime = 0;
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long lastDisplayUpdate = 0;
bool trackingVehicle = false;

void setup() {
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");
  delay(2000);
  lcd.clear();
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  bool isNight = (ldrValue < DARK_THRESHOLD); // Check if it's dark
  
  // Most IR sensors output LOW (0) when an object is detected
  bool ir1Triggered = (digitalRead(IR1_PIN) == LOW); 
  bool ir2Triggered = (digitalRead(IR2_PIN) == LOW);

  // --- 1. DAYTIME MODE ---
  if (!isNight) {
    analogWrite(LED_PIN, 0); // LEDs OFF
    updateEnvironmentDisplay("Day Mode: OFF  ");
    trackingVehicle = false;
  } 
  
  // --- 2. NIGHTTIME MODE ---
  else {
    unsigned long currentTime = millis();

    // Motion Detection & Speed Tracking Logic
    if (ir1Triggered && !trackingVehicle) {
      time1 = millis();
      lastMotionTime = time1; // Reset dimming timer
      trackingVehicle = true;
    }
    else if (ir1Triggered) {
       lastMotionTime = millis(); // Keep timer fresh if someone is standing there
    }

    if (ir2Triggered && trackingVehicle) {
      time2 = millis();
      lastMotionTime = time2; // Reset dimming timer
      trackingVehicle = false; // End tracking
      
      // Calculate Speed
      float timeTakenSec = (time2 - time1) / 1000.0;
      if (timeTakenSec > 0) {
        float speedMPS = DISTANCE_METERS / timeTakenSec;
        float speedKMH = speedMPS * 3.6;

        // Display Speed & Violation
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Speed: ");
        lcd.print(speedKMH, 1);
        lcd.print(" km/h");
        
        if (speedKMH > SPEED_LIMIT_KMH) {
          lcd.setCursor(0, 1);
          lcd.print("! VIOLATION !");
        }
        delay(3000); // Hold alert on screen for 3 seconds
        lcd.clear();
      }
    } else if (ir2Triggered) {
       lastMotionTime = millis(); // Reset dimming timer if IR2 is triggered alone
    }

    // --- 3. ADAPTIVE DIMMING LOGIC ---
    unsigned long timeSinceLastMotion = millis() - lastMotionTime;

    if (timeSinceLastMotion < 5000) {
      // 0 to 5 seconds: 100% Brightness
      analogWrite(LED_PIN, 255); 
    } 
    else if (timeSinceLastMotion < 10000) {
      // 5 to 10 seconds: 50% Brightness
      analogWrite(LED_PIN, 127); 
    } 
    else {
      // > 10 seconds: 25% Brightness (Idle)
      analogWrite(LED_PIN, 64);  
    }

    // Update normal display if no speed alert is currently dominating the screen
    if (millis() - lastDisplayUpdate > 2000 && !trackingVehicle) {
      updateEnvironmentDisplay("Night: Idle/Act");
      lastDisplayUpdate = millis();
    }
  }
}

// --- Helper Function: Update LCD with DHT Data ---
void updateEnvironmentDisplay(String statusText) {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Check if any reads failed
  if (isnan(temp) || isnan(hum)) {
    lcd.setCursor(0, 0);
    lcd.print(statusText);
    lcd.setCursor(0, 1);
    lcd.print("DHT Sensor Error");
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print(statusText);
  lcd.setCursor(0, 1);
  lcd.print("T:"); lcd.print((int)temp); lcd.print("C ");
  lcd.print("H:"); lcd.print((int)hum); lcd.print("%  ");
}
