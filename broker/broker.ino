#include "TinyMqtt.h"   // https://github.com/hsaturn/TinyMqtt
#include <SPI.h>
#include <WiFi.h>
// #include <Ethernet.h>

const int LED_PIN = 2;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
/*byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};*/

// IPAddress ip(192, 168, 2, 177);
// EthernetServer server(80);

// Set web server port number to 80
WiFiServer server(80);

#define PORT 1883
MqttBroker broker(PORT);
MqttClient mqtt_client(&broker);

const char* ssid = "CASARTORI";
const char* password = "sabukas99";



// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

char* sensorValue = "0";

void onPublish(const MqttClient*, const Topic& topic, const char* payload, size_t) { 
  Serial.print("--> client A received ");
  Serial.print(topic.c_str());
  Serial.print(", ");
  Serial.println(payload);

  sensorValue = (char*)payload;

  Serial.print("SENSOR VALUE: ");
  Serial.println(sensorValue);
  
   blinkLed();
}


void setup() {
  Serial.begin(115200);
  setupWifi();
  //setupWebServer();
  setupBroker();
  server.begin();

  pinMode(LED_PIN, OUTPUT);
}

void setupWifi() {
  if (strlen(ssid) == 0){
    Serial.println("****** PLEASE MODIFY ssid/password *************");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {   
    Serial.print(".");
    delay(500);
  }
  
  Serial.print("\nConnected to ");
  Serial.print(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}

void setupBroker() {
  broker.begin();
  mqtt_client.setCallback(onPublish);
  mqtt_client.subscribe("test/test");
  
  Serial.print("Broker ready : ");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(PORT);
}

void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(300);                  // wait for a second
  digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(300);  
}

void executeBroker() {
  broker.loop();
  mqtt_client.loop();  // Don't forget to call loop() for each broker and client
}

void executeWebServer(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              // output26State = "on";
              //digitalWrite(output26, HIGH);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.print("<h2>");
            client.print(sensorValue);
            client.println("</h2></body>");
            client.println("<script>window.onload = function(e) { setTimeout(() => { location.reload(); }, 5000); }; </script>");
            client.println("</html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void loop(){
  executeBroker();
  executeWebServer();
}
