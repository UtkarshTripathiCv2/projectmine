/*
 * ============================================================
 *  ESP32 ROBOT DASHBOARD + MOTOR CONTROL
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// ---------------- WIFI AP ----------------
const char* AP_SSID = "TERRAAA";
const char* AP_PASSWORD = "robot1234";

// ---------------- MOTOR PINS ----------------
#define IN1_PIN 25
#define IN2_PIN 26
#define IN3_PIN 27
#define IN4_PIN 14

#define ENA_PIN 32
#define ENB_PIN 33

// ---------------- UART ----------------
#define RX2_PIN 16
#define TX2_PIN 17

// ---------------- PWM ----------------
#define PWM_FREQ 1000
#define PWM_RES 8
#define MOTOR_SPEED 200

WebServer server(80);

String uartBuffer = "";

// ============================================================
// SENSOR STRUCT
// ============================================================

struct SensorData {

  float temperature = 0;
  float humidity = 0;

  int gasRaw = 0;
  int flamePct = 0;
  int rainPct = 0;
  int soilPct = 0;

} sensors;

// ============================================================
// MOTOR FUNCTIONS
// ============================================================

void motorsStop() {

  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);

  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);

  ledcWrite(ENA_PIN, 0);
  ledcWrite(ENB_PIN, 0);
}

void motorsForward() {

  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);

  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);

  ledcWrite(ENA_PIN, MOTOR_SPEED);
  ledcWrite(ENB_PIN, MOTOR_SPEED);
}

void motorsBackward() {

  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);

  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);

  ledcWrite(ENA_PIN, MOTOR_SPEED);
  ledcWrite(ENB_PIN, MOTOR_SPEED);
}

void motorsLeft() {

  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);

  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);

  ledcWrite(ENA_PIN, MOTOR_SPEED);
  ledcWrite(ENB_PIN, MOTOR_SPEED);
}

void motorsRight() {

  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);

  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);

  ledcWrite(ENA_PIN, MOTOR_SPEED);
  ledcWrite(ENB_PIN, MOTOR_SPEED);
}

// ============================================================
// JSON PARSER
// ============================================================

void parseUARTLine(String line) {

  line.trim();

  Serial.print("Received: ");
  Serial.println(line);

  StaticJsonDocument<256> doc;

  auto error = deserializeJson(doc, line);

  if (error) {

    Serial.print("JSON Error: ");
    Serial.println(error.c_str());

    return;
  }

  sensors.temperature = doc["T"] | 0;
  sensors.humidity = doc["H"] | 0;

  sensors.gasRaw = doc["G"] | 0;
  sensors.flamePct = doc["F"] | 0;
  sensors.rainPct = doc["R"] | 0;
  sensors.soilPct = doc["S"] | 0;
}

// ============================================================
// API HANDLERS
// ============================================================

void handleSensors() {

  StaticJsonDocument<256> doc;

  doc["temperature"] = sensors.temperature;
  doc["humidity"] = sensors.humidity;

  doc["gasRaw"] = sensors.gasRaw;
  doc["flamePct"] = sensors.flamePct;
  doc["rainPct"] = sensors.rainPct;
  doc["soilPct"] = sensors.soilPct;

  String json;

  serializeJson(doc, json);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleMotor() {

  String cmd = server.arg("cmd");

  if (cmd == "forward") motorsForward();
  else if (cmd == "backward") motorsBackward();
  else if (cmd == "left") motorsLeft();
  else if (cmd == "right") motorsRight();
  else motorsStop();

  server.send(200, "text/plain", "OK");
}

// ============================================================
// WEB DASHBOARD
// ============================================================

void handleRoot() {

  String html = R"rawliteral(

<!DOCTYPE html>
<html>
<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{
  background:#0f172a;
  color:white;
  font-family:Arial;
  text-align:center;
}

.grid{
  display:grid;
  grid-template-columns:repeat(2,1fr);
  gap:10px;
  margin-top:20px;
}

.card{
  background:#1e293b;
  padding:20px;
  border-radius:15px;
}

.val{
  font-size:28px;
  font-weight:bold;
}

button{
  width:80px;
  height:80px;
  font-size:30px;
  margin:5px;
  border:none;
  border-radius:15px;
}

</style>

</head>

<body>

<h2>ESP32 Robot Dashboard</h2>

<div class="grid">

<div class="card">
Temp
<div class="val" id="t">--</div>
</div>

<div class="card">
Humidity
<div class="val" id="h">--</div>
</div>

<div class="card">
Gas
<div class="val" id="g">--</div>
</div>

<div class="card">
Soil
<div class="val" id="s">--</div>
</div>

<div class="card">
Fire
<div class="val" id="f">--</div>
</div>

<div class="card">
Rain
<div class="val" id="r">--</div>
</div>

</div>

<br><br>

<button onclick="move('forward')">↑</button>
<br>

<button onclick="move('left')">←</button>

<button onclick="move('stop')">■</button>

<button onclick="move('right')">→</button>

<br>

<button onclick="move('backward')">↓</button>

<script>

async function updateData(){

  let res = await fetch('/api/sensors');

  let d = await res.json();

  document.getElementById('t').innerHTML = d.temperature;
  document.getElementById('h').innerHTML = d.humidity;

  document.getElementById('g').innerHTML = d.gasRaw;
  document.getElementById('s').innerHTML = d.soilPct;

  document.getElementById('f').innerHTML = d.flamePct;
  document.getElementById('r').innerHTML = d.rainPct;
}

setInterval(updateData,1000);

function move(cmd){

  fetch('/api/motor?cmd=' + cmd);
}

</script>

</body>
</html>

)rawliteral";

  server.send(200, "text/html", html);
}

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  ledcAttach(ENA_PIN, PWM_FREQ, PWM_RES);
  ledcAttach(ENB_PIN, PWM_FREQ, PWM_RES);

  motorsStop();

  WiFi.softAP(AP_SSID, AP_PASSWORD);

  Serial.println();
  Serial.println("WiFi Started");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/api/sensors", handleSensors);
  server.on("/api/motor", handleMotor);

  server.begin();
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  server.handleClient();

  while (Serial2.available()) {

    char c = Serial2.read();

    if (c == '\n') {

      parseUARTLine(uartBuffer);

      uartBuffer = "";
    }
    else if (c != '\r') {

      uartBuffer += c;
    }
  }
}
