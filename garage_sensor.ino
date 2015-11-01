/*

Turns ESP8266 into a garage door sensor.

 - publishes to "garage" via mqtt.
 - Uses simple digital sensor.

Requires library from https://github.com/Imroy/pubsubclient

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid =  "-------";    // cannot be longer than 32 characters!
const char *pass =  "-------";    //

// Update these with values suitable for your network.
IPAddress server(192, 168, 0, 100);   // Ip address from the CloudMQTT server.
const uint16_t port = 1883;           // change for CloudMQTT.

bool garage_open = false;
bool mqtt_connected = false;

// the pin that has the sensor to detect the garage door state.
const uint16_t sensor = 2;

void callback(const MQTT::Publish& pub) {
  // handle message arrived
}

WiFiClient wclient;
PubSubClient client(wclient, server, port);

void setup() {
  pinMode(sensor, INPUT);
  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();

  client.set_callback(callback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect(MQTT::Connect("<clientID>").set_auth("<username>","<password>"))) {
        Serial.print("connected to MQTT");
      }
    }

    if (client.connected())
    {
      mqtt_connected = true;
      if (digitalRead(sensor))
      {
        if (garage_open)
        {
          Serial.println("Closed");
          client.publish("garage", "Closed");
        }
        garage_open = false;
      }
      else
      {
        // door open
        if (!garage_open)
        {
          Serial.println("Opened");
          client.publish("garage", "Open");
        }
        garage_open = true;
      }
    }
  }
}
