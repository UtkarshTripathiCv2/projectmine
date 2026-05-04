#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>

// RX = 13, TX = 14 (TX 14 is unused but needed by the library)
SoftwareSerial ArduinoSerial(13, 14); 
WebServer server(80);

float temp = 0, hum = 0;
int gas = 0, rain = 0, soil = 0, flame = 0;

String getHTML() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{font-family:Arial; text-align:center; background:#f4f7f6; color:#333;}";
  html += ".grid{display:grid; grid-template-columns: repeat(auto-fit, minmax(120px, 1fr)); gap:10px; padding:20px;}";
  html += ".card{background:white; padding:15px; border-radius:10px; box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  html += "h1{color:#007bff;} .val{font-size:1.5rem; font-weight:bold; color:#007bff;}</style>";
  html += "<script>setInterval(function(){fetch('/data').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('t').innerText=d.t; document.getElementById('h').innerText=d.h;";
  html += "document.getElementById('g').innerText=d.g; document.getElementById('r').innerText=d.r;";
  html += "document.getElementById('s').innerText=d.s; document.getElementById('f').innerText=d.f;});}, 2000);</script>";
  html += "</head><body><h1>Sensor Hub Dashboard</h1><div class='grid'>";
  html += "<div class='card'>Temp<br><span class='val' id='t'>0</span> C</div>";
  html += "<div class='card'>Hum<br><span class='val' id='h'>0</span> %</div>";
  html += "<div class='card'>Gas<br><span class='val' id='g'>0</span></div>";
  html += "<div class='card'>Rain<br><span class='val' id='r'>0</span></div>";
  html += "<div class='card'>Soil<br><span class='val' id='s'>0</span></div>";
  html += "<div class='card'>Flame<br><span class='val' id='f'>0</span></div>";
  html += "</div></body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);        // USB Debugging
  ArduinoSerial.begin(9600);   // Arduino Input
  
  WiFi.softAP("ESP32_Dashboard", "12345678");
  Serial.println("Dashboard Ready!");
  Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());

  server.on("/", [](){ server.send(200, "text/html", getHTML()); });
  server.on("/data", [](){
    String json = "{\"t\":\""+String(temp,1)+"\",\"h\":\""+String(hum,1)+"\",\"g\":\""+String(gas)+"\",\"r\":\""+String(rain)+"\",\"s\":\""+String(soil)+"\",\"f\":\""+String(flame)+"\"}";
    server.send(200, "application/json", json);
  });
  server.begin();
}

void loop() {
  server.handleClient();

  if (ArduinoSerial.available()) {
    String data = ArduinoSerial.readStringUntil('\n');
    Serial.println("Received: " + data); // Check this in Serial Monitor

    int c1 = data.indexOf(',');
    int c2 = data.indexOf(',', c1+1);
    int c3 = data.indexOf(',', c2+1);
    int c4 = data.indexOf(',', c3+1);
    int c5 = data.indexOf(',', c4+1);

    if (c5 != -1) {
      temp = data.substring(0, c1).toFloat();
      hum = data.substring(c1+1, c2).toFloat();
      gas = data.substring(c2+1, c3).toInt();
      rain = data.substring(c3+1, c4).toInt();
      soil = data.substring(c4+1, c5).toInt();
      flame = data.substring(c5+1).toInt();
    }
  }
}
