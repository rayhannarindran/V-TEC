import paho.mqtt.client as mqtt

client = mqtt.Client()
client.connect("192.168.1.13", 1883, 60)

while True:
    payload = input("Enter payload: ")

    print("Publishing message...")
    client.publish("test", payload)
    print("Message published!")