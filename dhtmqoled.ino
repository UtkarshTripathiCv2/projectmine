#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configuration ---
const char* ssid = "ESP32_Monitor";
const char* password = "password123";

#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ_PIN 34 

// OLED Settings
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
// Use -1 if your OLED doesn't have a RESET pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// Global variables for sensor data
float temp = 0;
float hum = 0;
int gas = 0;
unsigned long lastUpdate = 0;

// --- Web Dashboard HTML ---
String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{font-family:sans-serif; text-align:center; background:#f4f4f4; padding:20px;}";
  html += ".card{background:white; padding:20px; margin:10px; border-radius:10px; box-shadow:0 4px 8px rgba(0,0,0,0.1); display:inline-block; width:150px;}";
  html += "h1{color:#333;} .val{font-size:1.5rem; font-weight:bold; color:#007bff;}</style>";
  html += "<script>setInterval(function(){fetch('/data').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('t').innerText=d.t; document.getElementById('h').innerText=d.h;";
  html += "document.getElementById('m').innerText=d.m;});}, 2000);</script>";
  html += "</head><body><h1>ESP32 Environment</h1>";
  html += "<div class='card'>Temp<br><span class='val' id='t'>0</span> &deg;C</div>";
  html += "<div class='card'>Hum<br><span class='val' id='h'>0</span> %</div>";
  html += "<div class='card'>Gas Raw<br><span class='val' id='m'>0</span></div>";
  html += "<p>Connected to ESP32 Hotspot</p></body></html>";
  return html;
}

// --- Function to Refresh OLED ---
void updateOLED() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  // Header
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.print("DASHBOARD LIVE");
  display.drawLine(0, 12, 128, 12, WHITE);
  
  // Temperature
  display.setCursor(0, 20);
  display.print("Temp: ");
  display.setTextSize(2);
  display.print(temp, 1); 
  display.setTextSize(1);
  display.print(" C");
  
  // Humidity
  display.setCursor(0, 40);
  display.setTextSize(1);
  display.print("Hum:  ");
  display.print(hum, 1);
  display.print(" %");
  
  // Gas
  display.setCursor(0, 53);
  display.print("Gas Reading: ");
  display.print(gas);
  
  display.display();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize OLED (Address 0x3C is standard)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println("Starting AP...");
  display.display();

  // Start Hotspot
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(IP);

  // Web Server Routes
  server.on("/", [](){ server.send(200, "text/html", getHTML()); });
  server.on("/data", [](){
    String json = "{\"t\":\""+String(temp,1)+"\",\"h\":\""+String(hum,1)+"\",\"m\":\""+String(gas)+"\"}";
    server.send(200, "application/json", json);
  });

  server.begin();
  
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("System Ready!");
  display.print("IP: ");
  display.println(IP);
  display.display();
  delay(2000);
}

void loop() {
  // Handle web requests
  server.handleClient();

  // Update logic every 2 seconds (Non-blocking)
  if (millis() - lastUpdate > 2000) {
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    gas = analogRead(MQ_PIN);
    
    // Safety check if sensor is unplugged
    if (isnan(temp)) temp = 0;
    if (isnan(hum)) hum = 0;

    updateOLED();
    lastUpdate = millis();
  }
}
