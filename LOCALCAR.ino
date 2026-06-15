#include <WiFi.h>
#include <WebServer.h>

// --- Wi-Fi Hotspot Credentials ---
const char* ssid = "ESP32_Rover";
const char* password = "password123"; // Must be at least 8 characters

// --- Web Server on Port 80 ---
WebServer server(80);

// --- Right Motor Pins ---
const int enableRightMotor = 22; 
const int rightMotorPin1 = 16;
const int rightMotorPin2 = 17;

// --- Left Motor Pins ---
const int enableLeftMotor = 23;
const int leftMotorPin1 = 18;
const int leftMotorPin2 = 19;

#define MAX_MOTOR_SPEED 255

// --- PWM Settings (Core 2.0.7) ---
const int PWMFreq = 1000;
const int PWMResolution = 8;
const int rightMotorChannel = 4;
const int leftMotorChannel = 5;

// ==========================================
// HTML & JS Webpage (Stored in ESP32 Memory)
// ==========================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background-color: #222; color: white; margin-top: 50px; user-select: none; -webkit-user-select: none; }
    .d-pad { display: flex; flex-direction: column; align-items: center; gap: 10px; margin-top: 30px; }
    .row { display: flex; gap: 10px; }
    .btn { width: 90px; height: 90px; font-size: 20px; font-weight: bold; border-radius: 15px; border: none; background-color: #007bff; color: white; touch-action: manipulation; box-shadow: 0 5px #0056b3; }
    .btn:active { background-color: #0056b3; box-shadow: 0 2px #004085; transform: translateY(3px); }
    .btn-stop { background-color: #dc3545; box-shadow: 0 5px #a71d2a; }
    .btn-stop:active { background-color: #a71d2a; box-shadow: 0 2px #7c151f; transform: translateY(3px); }
  </style>
  <script>
    // Sends a quick background request to the ESP32
    function sendCmd(cmd) {
      fetch('/' + cmd);
    }
  </script>
</head>
<body>
  <h2>ESP32 Wi-Fi Rover</h2>
  <div class="d-pad">
    <div class="row">
      <button class="btn" 
        onmousedown="sendCmd('forward')" onmouseup="sendCmd('stop')" onmouseleave="sendCmd('stop')"
        ontouchstart="sendCmd('forward')" ontouchend="sendCmd('stop')">FWD</button>
    </div>
    
    <div class="row">
      <button class="btn" 
        onmousedown="sendCmd('left')" onmouseup="sendCmd('stop')" onmouseleave="sendCmd('stop')"
        ontouchstart="sendCmd('left')" ontouchend="sendCmd('stop')">LEFT</button>
        
      <button class="btn btn-stop" onclick="sendCmd('stop')">STOP</button>
      
      <button class="btn" 
        onmousedown="sendCmd('right')" onmouseup="sendCmd('stop')" onmouseleave="sendCmd('stop')"
        ontouchstart="sendCmd('right')" ontouchend="sendCmd('stop')">RIGHT</button>
    </div>
    
    <div class="row">
      <button class="btn" 
        onmousedown="sendCmd('backward')" onmouseup="sendCmd('stop')" onmouseleave="sendCmd('stop')"
        ontouchstart="sendCmd('backward')" ontouchend="sendCmd('stop')">REV</button>
    </div>
  </div>
</body>
</html>
)rawliteral";

// ==========================================
// Motor Logic
// ==========================================
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);    
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);      
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);      
  }
  
  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);    
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);      
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);      
  }
  
  ledcWrite(rightMotorChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorChannel, abs(leftMotorSpeed));  
}

// ==========================================
// Web Server Route Handlers
// ==========================================
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleForward() {
  rotateMotor(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  server.send(200, "text/plain", "Forward");
}

void handleBackward() {
  rotateMotor(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
  server.send(200, "text/plain", "Backward");
}

void handleLeft() {
  rotateMotor(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
  server.send(200, "text/plain", "Left");
}

void handleRight() {
  rotateMotor(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  server.send(200, "text/plain", "Right");
}

void handleStop() {
  rotateMotor(0, 0);
  server.send(200, "text/plain", "Stopped");
}

// ==========================================
// Main Setup & Loop
// ==========================================
void setup() {
  Serial.begin(115200);

  // Setup Motor Pins
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // Setup PWM 
  ledcSetup(rightMotorChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enableRightMotor, rightMotorChannel);
  ledcAttachPin(enableLeftMotor, leftMotorChannel);
  rotateMotor(0, 0); 

  // Create Wi-Fi Hotspot (Access Point)
  Serial.println("Starting Wi-Fi Access Point...");
  WiFi.softAP(ssid, password);
  
  // IP Address is always 192.168.4.1 by default
  Serial.print("Connect to Wi-Fi: ");
  Serial.println(ssid);
  Serial.println("Then open a web browser and go to: http://192.168.4.1");

  // Assign routes to handlers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/forward", HTTP_GET, handleForward);
  server.on("/backward", HTTP_GET, handleBackward);
  server.on("/left", HTTP_GET, handleLeft);
  server.on("/right", HTTP_GET, handleRight);
  server.on("/stop", HTTP_GET, handleStop);

  // Start the server
  server.begin();
}

void loop() {
  // Constantly listen for HTTP requests from the phone
  server.handleClient();
}
