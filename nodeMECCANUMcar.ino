#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- PIN DEFINITIONS ---
const int RPWM_FL = D0; const int LPWM_FL = D1; 
const int RPWM_FR = D2; const int LPWM_FR = D3; 
const int RPWM_RL = D5; const int LPWM_RL = D6; 
const int RPWM_RR = D7; const int LPWM_RR = D8; 

// Hotspot Credentials
const char* ssid = "Mecanum-Rover";
const char* password = "password123";

ESP8266WebServer server(80);

int motorSpeed = 800; // Speed (0-1023)

// --- HTML Dashboard ---
String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'><style>"
"button{width:90px;height:90px;margin:10px;font-size:18px;border-radius:10px;background:#333;color:white;touch-action:manipulation;}"
"body{text-align:center;font-family:Arial;background:#f0f0f0;}"
".container{display:inline-block;max-width:400px;}</style></head><body>"
"<h1>Mecanum Control</h1><div class='container'>"
"<button onmousedown='f()' onmouseup='s()' ontouchstart='f()' ontouchend='s()'>UP</button><br>"
"<button onmousedown='sl()' onmouseup='s()' ontouchstart='sl()' ontouchend='s()'>S-LFT</button>"
"<button onmousedown='s()' ontouchstart='s()'>STOP</button>"
"<button onmousedown='sr()' onmouseup='s()' ontouchstart='sr()' ontouchend='s()'>S-RGT</button><br>"
"<button onmousedown='b()' onmouseup='s()' ontouchstart='b()' ontouchend='s()'>DOWN</button><br><br>"
"<button onmousedown='tl()' onmouseup='s()' ontouchstart='tl()' ontouchend='s()'>T-LFT</button>"
"<button onmousedown='tr()' onmouseup='s()' ontouchstart='tr()' ontouchend='s()'>T-RGT</button>"
"</div><script>"
"function send(p){fetch('/'+p);}"
"function f(){send('F');} function b(){send('B');} function sl(){send('SL');} function sr(){send('SR');}"
"function tl(){send('TL');} function tr(){send('TR');} function s(){send('S');}"
"</script></body></html>";

void setup() {
  Serial.begin(115200);
  
  pinMode(RPWM_FL, OUTPUT); pinMode(LPWM_FL, OUTPUT);
  pinMode(RPWM_FR, OUTPUT); pinMode(LPWM_FR, OUTPUT);
  pinMode(RPWM_RL, OUTPUT); pinMode(LPWM_RL, OUTPUT);
  pinMode(RPWM_RR, OUTPUT); pinMode(LPWM_RR, OUTPUT);
  stopRobot();

  WiFi.softAP(ssid, password);
  Serial.println("Hotspot Started: " + String(ssid));
  Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());

  server.on("/", []() { server.send(200, "text/html", html); });
  server.on("/F",  []() { moveForward();  server.send(200); });
  server.on("/B",  []() { moveBackward(); server.send(200); });
  server.on("/SL", []() { slideLeft();    server.send(200); });
  server.on("/SR", []() { slideRight();   server.send(200); });
  server.on("/TL", []() { turnLeft();     server.send(200); });
  server.on("/TR", []() { turnRight();    server.send(200); });
  server.on("/S",  []() { stopRobot();    server.send(200); });

  server.begin();
}

void loop() {
  server.handleClient();
}

// --- MOTION LOGIC ---
void moveForward() {
  analogWrite(RPWM_FL, motorSpeed); analogWrite(LPWM_FL, 0);
  analogWrite(RPWM_FR, motorSpeed); analogWrite(LPWM_FR, 0);
  analogWrite(RPWM_RL, motorSpeed); analogWrite(LPWM_RL, 0);
  analogWrite(RPWM_RR, motorSpeed); analogWrite(LPWM_RR, 0);
}

void moveBackward() {
  analogWrite(RPWM_FL, 0); analogWrite(LPWM_FL, motorSpeed);
  analogWrite(RPWM_FR, 0); analogWrite(LPWM_FR, motorSpeed);
  analogWrite(RPWM_RL, 0); analogWrite(LPWM_RL, motorSpeed);
  analogWrite(RPWM_RR, 0); analogWrite(LPWM_RR, motorSpeed);
}

void slideLeft() {
  analogWrite(RPWM_FL, 0);          analogWrite(LPWM_FL, motorSpeed);
  analogWrite(RPWM_FR, motorSpeed); analogWrite(LPWM_FR, 0);
  analogWrite(RPWM_RL, motorSpeed); analogWrite(LPWM_RL, 0);
  analogWrite(RPWM_RR, 0);          analogWrite(LPWM_RR, motorSpeed);
}

void slideRight() {
  analogWrite(RPWM_FL, motorSpeed); analogWrite(LPWM_FL, 0);
  analogWrite(RPWM_FR, 0);          analogWrite(LPWM_FR, motorSpeed);
  analogWrite(RPWM_RL, 0);          analogWrite(LPWM_RL, motorSpeed);
  analogWrite(RPWM_RR, motorSpeed); analogWrite(LPWM_RR, 0);
}

void turnLeft() { // Tank Turn Left
  analogWrite(RPWM_FL, 0);          analogWrite(LPWM_FL, motorSpeed);
  analogWrite(RPWM_RL, 0);          analogWrite(LPWM_RL, motorSpeed);
  analogWrite(RPWM_FR, motorSpeed); analogWrite(LPWM_FR, 0);
  analogWrite(RPWM_RR, motorSpeed); analogWrite(LPWM_RR, 0);
}

void turnRight() { // Tank Turn Right
  analogWrite(RPWM_FL, motorSpeed); analogWrite(LPWM_FL, 0);
  analogWrite(RPWM_RL, motorSpeed); analogWrite(LPWM_RL, 0);
  analogWrite(RPWM_FR, 0);          analogWrite(LPWM_FR, motorSpeed);
  analogWrite(RPWM_RR, 0);          analogWrite(LPWM_RR, motorSpeed);
}

void stopRobot() {
  analogWrite(RPWM_FL, 0); analogWrite(LPWM_FL, 0);
  analogWrite(RPWM_FR, 0); analogWrite(LPWM_FR, 0);
  analogWrite(RPWM_RL, 0); analogWrite(LPWM_RL, 0);
  analogWrite(RPWM_RR, 0); analogWrite(LPWM_RR, 0);
}
