#include "TinyMqtt.h"   // https://github.com/hsaturn/TinyMqtt
#include <SPI.h>
#include <WiFi.h>
#include <Ethernet.h>

const int LED_PIN = 2;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
/*byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};*/

// IPAddress ip(192, 168, 2, 177);
// EthernetServer server(80);

#define PORT 1883
MqttBroker broker(PORT);
MqttClient mqtt_client(&broker);

const char* ssid = "CASARTORI";
const char* password = "sabukas99";

void onPublish(const MqttClient*, const Topic& topic, const char* payload, size_t) { 
  Serial.print("--> client A received ");
  Serial.print(topic.c_str());
  Serial.print(", ");
  Serial.println(payload);

   blinkLed();
}


void setup() {
  Serial.begin(115200);
  setupWifi();
  //setupWebServer();
  setupBroker();

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

/*void setupWebServer() {
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}*/

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

/*void executeWebServer() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}*/

void executeBroker() {
  broker.loop();
  mqtt_client.loop();  // Don't forget to call loop() for each broker and client

  // delay(1000);   please avoid usage of delay (see below how this done using next_send and millis())
  /*static auto next_send = millis();
  
  if (millis() > next_send) {
    next_send += 1000;

    if (not mqtt_client.connected()) {
      Serial.println(millis() + ": Not connected to broker");
      return;
    }

    auto rnd=random(100);
    float temp = 23.5f;

    if (rnd > 66) temp += 0.1;
    else if (rnd < 33) temp -= 0.1;

    Serial.print("--> Publishing a new sensor/temperature value: ");
    Serial.println(temp);
    
    mqtt_client.publish("test/test", String(temp));
  }*/
}

void loop(){
  executeBroker();
  // executeWebServer();
}
