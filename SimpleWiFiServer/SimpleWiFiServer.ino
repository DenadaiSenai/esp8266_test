#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 4  // what pin we're connected to
#define LED 2     // Builtin LED
#define LVERDE 5
#define TIME 100
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11  // DHT 11
#define DHTTYPE DHT22  // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Wifi config
const char* ssid = "Marcio";
const char* password = "";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(LVERDE, OUTPUT);  // set the LED pin mode
  pinMode(LED, OUTPUT);
    delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  dht.begin();  // sensor de temperatura
}

void loop() {
  digitalWrite(LED, LOW);
  delay(TIME);

  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("<html><style>margin:0px auto;font-size:48px;border:1px solid red;</style><body>");

            // the content of the HTTP response follows the header:
            client.print("Ligar <a href=\"/H\">LED VERDE</a><br>");
            client.print("Desligar <a href=\"/L\">LED VERDE</a><br>");

            client.print("<h3 id=\"T\"></h3><br>");
            client.print("<h3 id=\"U\"></h3><br>");
            client.print("</body></html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LVERDE, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LVERDE, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    digitalWrite(LED, HIGH);
    delay(TIME);
  }
}
