#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// --- Configuration ---
const char* ssid = "ESP32_Monitor";
const char* password = "password123";

#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ_PIN 34 

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

// HTML Dashboard
String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Dashboard</title>";
  html += "<style>body{font-family:sans-serif; text-align:center; background:#f4f4f4;}";
  html += ".card{background:white; padding:20px; margin:15px; border-radius:10px; box-shadow:0 4px 8px rgba(0,0,0,0.1); display:inline-block; width:200px;}";
  html += "h1{color:#333;} .val{font-size:1.5rem; font-weight:bold; color:#007bff;}</style>";
  html += "<script>setInterval(function(){fetch('/data').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('t').innerText=d.t; document.getElementById('h').innerText=d.h;";
  html += "document.getElementById('m').innerText=d.m;});}, 2000);</script>";
  html += "</head><body><h1>ESP32 Local Sensor</h1>";
  html += "<div class='card'>Temp<br><span class='val' id='t'>--</span> &deg;C</div>";
  html += "<div class='card'>Humidity<br><span class='val' id='h'>--</span> %</div>";
  html += "<div class='card'>Gas Raw<br><span class='val' id='m'>--</span></div>";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int m = analogRead(MQ_PIN);
  
  String json = "{";
  json += "\"t\":\"" + String(isnan(t) ? 0 : t) + "\",";
  json += "\"h\":\"" + String(isnan(h) ? 0 : h) + "\",";
  json += "\"m\":\"" + String(m) + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Hotspot Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define Routes
  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();
}

void loop() {
  server.handleClient();
}
