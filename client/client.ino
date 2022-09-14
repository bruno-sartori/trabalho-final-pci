#include <WiFi.h>
#include <TinyMqtt.h>

const char* ssid = "CASARTORI";
const char* password =  "sabukas99";

MqttClient mqtt_client;

int SENSOR_PORT = 36;
int LED_PORT = 2;

int count = 0;

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

  pinMode(LED_PORT, OUTPUT);
  pinMode(SENSOR_PORT, INPUT);

  mqtt_client.connect("192.168.2.138", 1883);
}

void executeMqttClient() {
  int luminosity = analogRead(SENSOR_PORT);

  if (luminosity > 500) {
    digitalWrite(LED_PORT, HIGH);
  } else {
    digitalWrite(LED_PORT, LOW);  
  }
  
  Serial.println(luminosity);
  mqtt_client.publish("test/test", String(luminosity));
}

void loop() {
  count++;
  executeMqttClient();
  delay(500);
}
