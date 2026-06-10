#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_CAR";
const char* password = "12345678";

WebServer server(80);

// Right Motor
const int ENA = 25;
const int IN1 = 16;
const int IN2 = 17;

// Left Motor
const int ENB = 26;
const int IN3 = 18;
const int IN4 = 19;

void stopCar()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
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
button{
width:120px;
height:60px;
font-size:20px;
margin:10px;
}
body{
text-align:center;
font-family:Arial;
}
</style>
</head>
<body>

<h2>ESP32 Car Control</h2>

<p><a href="/forward"><button>Forward</button></a></p>

<p>
<a href="/left"><button>Left</button></a>
<a href="/stop"><button>Stop</button></a>
<a href="/right"><button>Right</button></a>
</p>

<p><a href="/backward"><button>Backward</button></a></p>

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

  analogWrite(ENA, 150);
  analogWrite(ENB, 150);

  stopCar();

  WiFi.softAP(ssid, password);

  Serial.println("Hotspot Started");
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
}

void loop()
{
  server.handleClient();
}
