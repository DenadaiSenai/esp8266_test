/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define SERIAL_BAUDRATE 2000000 // Configuração da velocidade da porta serial 2MBits

#define LED 2
#define led LED

const char *ssid = "IOT101";
const char *password = "Senai101!@#";

#define HTTP_PORT 80
#define DNS_PORT 53
IPAddress apIP(10, 10, 0, 1);
DNSServer dnsServer;

WebServer server(HTTP_PORT);

void handleRootOld()
{
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60);
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleRoot()
{
  const String index_html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>ESP8266 Dashboard</title><style>*{margin:0 auto;padding:0;border:1px solid red;align-items:center;text-align:center}nav ul{display:flex;flex-direction:row;margin:8px}header{background-color:orange}#temperature{font-size:96px;font-family:'Courier New',Courier,monospace;border:3px dotted #ff0;border-radius:24px;background-color:#9acd32}</style><script>setInterval(function(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById(\"temperature\").innerHTML=this.responseText)},e.open(\"GET\",\"/status\",!0),e.send()},5e3)</script></head><body><nav><ul><li>LED</li><li>RTC</li></ul></nav><header><h1>ESP 8266 DashBoard</h1></header><main id=\"temperature\">PRINCIPAL</main><footer><h3>©2023 Marcio Denadai</h3></footer></body></html>";
  server.send(200, "text/html", index_html);
}

void handleNotFound()
{
  digitalWrite(led, 1);
  server.send(404, "text/html", "<html style=\"*{margin: 0 auto;}\"><body><h1>PAGE NOT FOUND<br>404</h1></body></html>");
  digitalWrite(led, 0);
}

void wifi_apconfig()
{
  String MAC = WiFi.macAddress();
  MAC.replace(":", "");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("AP ESP " + MAC);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);
}

void status()
{
  long m = millis();
  randomSeed(m);
  long r = random(50);
  long umidade = random(100);
  String resposta = "{\"temperatura\":%T%, \"millis\":%M%, \"humidade\":%U%}";
  resposta.replace("%T%", String(r));
  resposta.replace("%M%", String(m));
  resposta.replace("%U%", String(umidade));
  server.send(200, "text/json", resposta);
}

void setup(void)
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(SERIAL_BAUDRATE);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    if (count_wifi < CLI_MAX_COUNT)
    {
      delay(500);
      Serial.print(".");
      count_wifi++;
    }
    else
    { // Se em 100 tentativas não conectar na rede cria um AP
      String msg = "Não foi possível conectar na rede %SSID%.\nConfigurando o ESP para modo Access Point (AP).";
      msg.replace("%SSID%", ssid);
      Serial.println("\nRede não encontrada.\n" + msg);
      delay(100);
      wifi_apconfig(); // Chama a rotina para configurar como Access Point
      ssid = WiFi.softAPSSID();
      break;
    }
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  // Old root (index.html)
  server.on("/old", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []()
            { server.send(200, "text/text", "this works as well"); });

  server.on("/", handleRoot); // New index_html
  server.on("/status", status);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  digitalWrite(LED, !digitalRead(LED));
  delay(100);
}

void drawGraph()
{
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10)
  {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
