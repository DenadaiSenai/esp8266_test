/*
   ESP8266 LLMNR responder sample
   Copyright (C) 2017 Stephen Warren <swarren@wwwdotorg.org>

   Based on:
   ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
   Version 1.1
   Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
   ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
   MDNS-SD Support 2015 Hristo Gochkov
   Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)

   License (MIT license):

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

*/
/*
   This is an example of an HTTP server that is accessible via http://esp8266/
   (or perhaps http://esp8266.local/) thanks to the LLMNR responder.

   Instructions:
   - Update WiFi SSID and password as necessary.
   - Flash the sketch to the ESP8266 board.
   - Windows:
     - No additional software is necessary.
     - Point your browser to http://esp8266/, you should see a response. In most
       cases, it is important that you manually type the "http://" to force the
       browser to search for a hostname to connect to, rather than perform a web
       search.
     - Alternatively, run the following command from the command prompt:
       ping esp8266
   - Linux:
     - To validate LLMNR, install the systemd-resolve utility.
     - Execute the following command:
       systemd-resolve -4 -p llmnr esp8266
     - It may be possible to configure your system to use LLMNR for all name
       lookups. However, that is beyond the scope of this description.

*/

#include <ESP8266WiFi.h>
#include <ESP8266LLMNR.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>

// #include "SoftwareSerial.h"
#define SERIAL_BAUDRATE 2000000

// Config DNS Server
// const byte DNS_PORT = 53;

#define HTTP_PORT 80 //
#define DNS_PORT 53
IPAddress apIP(10, 10, 0, 1);
DNSServer dnsServer;

// SoftwareSerial ESP_Serial(10, 11);  // RX, TX

#ifndef STASSID
#define STASSID "MEUWIFI"
#define STAPSK "senhasupersecreta"
#endif

#define LED 2 // Pino do LED embutido no NodeMCU ESP8266

#define CLI_MAX_COUNT 10 // Define o número máximo de tentativas para conectar como cliente na rede WiFi
char count_wifi = 0;     // Contador de tentativas de conexão com WiFi

String ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer web_server(HTTP_PORT);

const String index_html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>ESP8266 Dashboard</title><style>*{margin:0 auto;padding:0;border:1px solid red;align-items:center;text-align:center}nav ul{display:flex;flex-direction:row;margin:8px}header{background-color:orange}#temperature{font-size:96px;font-family:'Courier New',Courier,monospace;border:3px dotted #ff0;border-radius:24px;background-color:#9acd32}</style><script>setInterval(function(){var e=new XMLHttpRequest;e.onreadystatechange=function(){4==this.readyState&&200==this.status&&(document.getElementById(\"temperature\").innerHTML=this.responseText)},e.open(\"GET\",\"/status\",!0),e.send()},5e3)</script></head><body><nav><ul><li>LED</li><li>RTC</li></ul></nav><header><h1>ESP 8266 DashBoard</h1></header><main id=\"temperature\">PRINCIPAL</main><footer><h3>©2023 Marcio Denadai</h3></footer></body></html>";

void handle_http_not_found()
{
  web_server.send(404, "text/html", "<html style=\"*{margin: 0 auto;}\"><body><h1>PAGE NOT FOUND<br>404</h1></body></html>");
}

void handle_http_root()
{
  web_server.send(200, "text/html", index_html);
}

// WiFi AP config
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
  web_server.send(200, "text/json", resposta);
}

void setup(void)
{
  pinMode(LED, OUTPUT);
  randomSeed(millis());
  Serial.begin(SERIAL_BAUDRATE);
  // ESP_Serial.begin(2000000);

  // Connect to WiFi network
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
  // WiFi.printDiag(Serial); // Imprime o diagnotico do WiFi na serial
  Serial.println(apIP.toString());

  // Start LLMNR responder
  LLMNR.begin("esp8266");
  Serial.println("LLMNR responder started");
  // Start HTTP server
  web_server.onNotFound(handle_http_not_found);
  web_server.on("/", handle_http_root);
  web_server.on("/status", status);
  web_server.begin();
  Serial.println("HTTP server started");
  Serial.print("Size of INDEX_HTML: ");
  Serial.print(index_html.length());
  Serial.println(" bytes");
}

void loop(void)
{
  web_server.handleClient();

  digitalWrite(LED, !digitalRead(LED));
  delay(100);

  // if (ESP_Serial.available()) {
  //   Serial.write(ESP_Serial.read());
  // }
  // if (Serial.available()) {
  //   ESP_Serial.write(Serial.read());
  // }
}
