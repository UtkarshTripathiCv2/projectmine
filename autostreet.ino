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
const int SPEED_LIMIT_KMH = 30;     // Set your speed violation limit
const float DISTANCE_METERS = 0.30; // 40 cm distance between IR sensors

// New LDR Thresholds based on your readings
const int NIGHT_THRESHOLD = 500; // Above 500 is Night
const int DAY_THRESHOLD = 200;   // Below 200 is Absolute Day

// --- Objects & Variables ---
DHT dht(DHT_PIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 is the most common I2C address

unsigned long lastMotionTime = 0;
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastDebugUpdate = 0; // Timer for periodic serial monitor updates

bool trackingVehicle = false;
bool isNight = false; // Keeps track of current day/night state (Hysteresis)
int currentBrightness = 0; // To track and print the current LED PWM value

void setup() {
  Serial.begin(9600); // Initialize Serial Monitor
  Serial.println("===============================");
  Serial.println("Smart Street Light System Start");
  Serial.println("===============================");

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
  
  // --- Hysteresis Logic for Day/Night Detection ---
  if (ldrValue > NIGHT_THRESHOLD) {
    isNight = true;  // It's definitely dark
  } else if (ldrValue < DAY_THRESHOLD) {
    isNight = false; // It's definitely bright
  }
  // If it's between 200 and 500, isNight remains whatever it currently is.
  
  // Most IR sensors output LOW (0) when an object is detected
  bool ir1Triggered = (digitalRead(IR1_PIN) == LOW); 
  bool ir2Triggered = (digitalRead(IR2_PIN) == LOW);

  // --- 1. DAYTIME MODE ---
  if (!isNight) {
    currentBrightness = 0;
    analogWrite(LED_PIN, currentBrightness); // LEDs OFF
    updateEnvironmentDisplay("Day Mode: OFF  ");
    
    if (trackingVehicle) {
      Serial.println("[DEBUG] Transitioned to Day Mode. Tracking aborted.");
    }
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
      Serial.println("\n[EVENT] IR 1 Triggered: Tracking Vehicle...");
    }
    else if (ir1Triggered) {
       lastMotionTime = millis(); // Keep timer fresh if someone is standing there
    }

    if (ir2Triggered && trackingVehicle) {
      time2 = millis();
      lastMotionTime = time2; // Reset dimming timer
      trackingVehicle = false; // End tracking
      Serial.println("[EVENT] IR 2 Triggered: Tracking Complete.");
      
      // Calculate Speed
      float timeTakenSec = (time2 - time1) / 1000.0;
      if (timeTakenSec > 0) {
        float speedMPS = DISTANCE_METERS / timeTakenSec;
        float speedKMH = speedMPS * 3.6;

        // Print to Serial Monitor
        Serial.print("[SPEED] Time: "); Serial.print(timeTakenSec); Serial.print("s | ");
        Serial.print("Speed: "); Serial.print(speedKMH); Serial.println(" km/h");

        // Display Speed & Violation on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Speed: ");
        lcd.print(speedKMH, 1);
        lcd.print(" km/h");
        
        if (speedKMH > SPEED_LIMIT_KMH) {
          Serial.println("[ALERT] Speed Violation Detected!");
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
      currentBrightness = 255; // 0 to 5 seconds: 100% Brightness
    } 
    else if (timeSinceLastMotion < 10000) {
      currentBrightness = 127; // 5 to 10 seconds: 50% Brightness
    } 
    else {
      currentBrightness = 64;  // > 10 seconds: 25% Brightness (Idle)
    }
    analogWrite(LED_PIN, currentBrightness);

    // Update normal display if no speed alert is currently dominating the screen
    if (millis() - lastDisplayUpdate > 2000 && !trackingVehicle) {
      updateEnvironmentDisplay("Night: Idle/Act");
      lastDisplayUpdate = millis();
    }
  }

  // --- 4. PERIODIC SERIAL DEBUGGING ---
  // Print standard status every 1000ms (1 second) to avoid flooding the monitor
  if (millis() - lastDebugUpdate > 1000) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    Serial.print("[STATUS] ");
    Serial.print(isNight ? "NIGHT " : "DAY   ");
    Serial.print("| LDR: ");
    Serial.print(ldrValue);
    Serial.print(" | IR1: ");
    Serial.print(ir1Triggered ? "DET" : "CLR");
    Serial.print(" | IR2: ");
    Serial.print(ir2Triggered ? "DET" : "CLR");
    Serial.print(" | LED PWM: ");
    Serial.print(currentBrightness);
    
    // Handle DHT errors cleanly in serial
    if (isnan(temp) || isnan(hum)) {
      Serial.println(" | DHT: ERROR");
    } else {
      Serial.print(" | Temp: ");
      Serial.print(temp, 1);
      Serial.print("C | Hum: ");
      Serial.print(hum, 1);
      Serial.println("%");
    }
    
    lastDebugUpdate = millis();
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
