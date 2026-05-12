#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Robot_Project";
const char* password = "password123";

WebServer server(80);
String globalData = "0,0,0,0,0,0";

String getPage() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>body{font-family:sans-serif; text-align:center; background:#f0f0f0;}";
  html += ".card{display:inline-block; background:white; width:130px; margin:8px; padding:15px; border-radius:10px; box-shadow:0 4px 8px rgba(0,0,0,0.1);}";
  html += ".val{font-size:22px; color:#2196F3; font-weight:bold;} button{width:80px; height:50px; margin:5px; font-weight:bold;}</style></head><body>";
  html += "<h1>Live Robot Data</h1><div id='data_grid'>Loading...</div><hr>";
  html += "<h3>Control Pad</h3><button onclick=\"c('F')\">UP</button><br>";
  html += "<button onclick=\"c('L')\">LEFT</button><button onclick=\"c('S')\">STOP</button><button onclick=\"c('R')\">RIGHT</button><br>";
  html += "<button onclick=\"c('B')\">DOWN</button>";
  html += "<script>function c(v){fetch('/c?v='+v);} setInterval(function(){fetch('/d').then(r=>r.text()).then(d=>{";
  html += "if(d.length<5)return; let s=d.split(','); let h=''; let names=['Temp','Hum','Gas','Soil','Rain','Flame'];";
  html += "for(let i=0;i<6;i++){ h+='<div class=\"card\">'+names[i]+'<div class=\"val\">'+s[i]+'</div></div>'; }";
  html += "document.getElementById('data_grid').innerHTML=h;});}, 1000);</script></body></html>";
  return html;
}

void setup() {
  Serial.begin(115200); // For debugging
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Communication with Uno
  
  WiFi.softAP(ssid, password);
  Serial.println("IP: " + WiFi.softAPIP().toString());

  server.on("/", [](){ server.send(200, "text/html", getPage()); });
  server.on("/d", [](){ server.send(200, "text/plain", globalData); });
  server.on("/c", [](){ if(server.hasArg("v")) Serial2.print(server.arg("v")); server.send(200); });
  
  server.begin();
}

void loop() {
  server.handleClient();
  if (Serial2.available()) {
    globalData = Serial2.readStringUntil('\n');
    globalData.trim();
  }
}
