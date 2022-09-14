#include <WiFi.h>
#include <TinyMqtt.h>

const char* ssid = "CASARTORI";
const char* password =  "sabukas99";

MqttClient mqtt_client;

void setupWifi() {
  if (strlen(ssid) == 0) {
    Serial.println("***** PLEASE MODIFY WIFI CREDENTIALS");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected to: ");
  Serial.print(ssid);
  Serial.print(" IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  setupWifi();

  mqtt_client.connect("192.168.2.138", 1883);
}

void executeMqttClient() {
  mqtt_client.publish("test/test", "test client");
  Serial.println("Sending message");
}

void loop() {
  executeMqttClient();
  delay(10000);
}
