#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP8266_CAR";
const char* password = "12345678";

ESP8266WebServer server(80);

// Motor Pins
const int ENA = D1;   // GPIO5
const int IN1 = D2;   // GPIO4
const int IN2 = D3;   // GPIO0

const int ENB = D6;   // GPIO12
const int IN3 = D4;   // GPIO2
const int IN4 = D5;   // GPIO14

int speedValue = 700;      // PWM 0-1023

void stopCar()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void forward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
body{
font-family:Arial;
text-align:center;
background:#f5f5f5;
}

button{
width:130px;
height:60px;
font-size:20px;
margin:10px;
}
</style>

</head>

<body>

<h2>ESP8266 Car Control</h2>

<p>
<a href="/forward"><button>Forward</button></a>
</p>

<p>
<a href="/left"><button>Left</button></a>
<a href="/stop"><button>Stop</button></a>
<a href="/right"><button>Right</button></a>
</p>

<p>
<a href="/backward"><button>Backward</button></a>
</p>

</body>
</html>
)rawliteral";

void handleRoot()
{
  server.send(200, "text/html", webpage);
}

void setup()
{
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWriteRange(1023);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);

  stopCar();

  WiFi.softAP(ssid, password);

  Serial.println("WiFi Started");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);

  server.on("/forward", []() {
    forward();
    server.send(200, "text/plain", "Forward");
  });

  server.on("/backward", []() {
    backward();
    server.send(200, "text/plain", "Backward");
  });

  server.on("/left", []() {
    left();
    server.send(200, "text/plain", "Left");
  });

  server.on("/right", []() {
    right();
    server.send(200, "text/plain", "Right");
  });

  server.on("/stop", []() {
    stopCar();
    server.send(200, "text/plain", "Stop");
  });

  server.begin();

  Serial.println("Server Started");
}

void loop()
{
  server.handleClient();
}
