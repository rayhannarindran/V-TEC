#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>

const char* ssid = "Narindrans iPhone";
const char* password = "rayhan07";

const char* mqtt_server = "172.20.10.7";

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
}

void loop(){
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("Wifi is connected!");
      Serial.println(WiFi.localIP());
    }

    ping_server();

    if (!client.connected())
    {
      reconnect();
    }
    client.loop();

    client.publish("test", "This is a payload");
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