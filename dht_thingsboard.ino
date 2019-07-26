/*
  Example to send temperature & humidity from dht sensor to thingsboard using mqtt protocol
  dht       Esp8266
  Vcc------->3.3V
  Gnd------->G
  data------>D2
  https://demo.thingsboard.io/login
*/

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Update these with values suitable for your network.
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "demo.thingsboard.io";
const char* TOKEN = "YOUR_TOKEN";

long lastMsg = 0;



void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //get data from dht sensor
  Serial.println("Initializing dht");
  dht.begin();
  //connect to wifi
  Serial.println("initializing connection to WiFi");
  setup_wifi();
  //set mqtt server
  client.setServer(mqtt_server, 1883);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", TOKEN, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.println(h);
    Serial.println(t);
    String msg = "{\"temperature\":" + String(t) + "," + "\"humidity\":" + String(h) + "}";
    Serial.println("Publishing to thingsboard: ");
    Serial.println(msg);
    client.publish("v1/devices/me/attributes", msg.c_str());
  }
}
