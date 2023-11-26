#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

const char* ssid = "Narindrans iPhone";
const char* password = "rayhan07";
const char* mqtt_server = "172.20.10.10";

WiFiClient espClient;
PubSubClient client;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.
uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

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

    if (!mlx.begin()) {
      Serial.println("Error connecting to MLX sensor. Check wiring.");
      while (1);
    };

    // Initialize sensor
    if (!particleSensor.begin(Wire, 115200)) //Use default I2C port, 400kHz speed
    {
      Serial.println(F("MAX30105 was not found. Please check wiring/power."));
      while (1);
    }

    byte ledBrightness = 60; //Options: 0=Off to 255=50mA
    byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
    byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411; //Options: 69, 118, 215, 411
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

    Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
    Serial.println("================================================");
}

void loop(){
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();

    Serial.print(F("Object Temp = "));
    Serial.print(mlx.readObjectTempC());
    Serial.println(" C");

    Serial.print(F("Ambient Temp = "));
    Serial.print(mlx.readAmbientTempC());
    Serial.println(" C");

    client.publish("data", "This is a payload");
    delay(500);
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