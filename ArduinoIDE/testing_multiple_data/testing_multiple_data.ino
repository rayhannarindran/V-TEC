#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>

const char* ssid = "Narindrans iPhone";
const char* password = "rayhan07";

const char* mqtt_server = "172.20.10.10";

WiFiClient espClient;
PubSubClient client;

void setup(){
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    client.setClient(espClient);
    client.setServer(mqtt_server, 1883);
    delay(200);
    ping_server();
    delay(200);
}

void loop(){
    if (!client.connected())
    {
      reconnect();
    }

    client.loop();

    //Data Temporary
    StaticJsonDocument<80> doc;
    char output[80];

    //Input Data
    //TODO: masukkan reading sensor disini!
    float object_temp = 10;
    float ambient_temp = 15;
    float heart_rate = 90;
    float spo2 = 100;

    doc["ot"] = object_temp;
    doc["at"] = ambient_temp;
    doc["hr"] = heart_rate;
    doc["spo"] = spo2;

    //Serialized Json dan Sending
    serializeJson(doc, output);
    Serial.println(output);
    client.publish("data", output);
    delay(250);
}

void ping_server(){
    Serial.println("Testing Server Connection...");
    bool ping_server = Ping.ping(mqtt_server);
    
    if (ping_server){
      Serial.println("Connected to server");
    } else {
      Serial.println("NOT Connected to server");
    }
}

void reconnect(){
  while (!client.connected())
  {
    Serial.println("Attempting Connection...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    ping_server();

    if (client.connect(clientId.c_str()))
    {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, code=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
