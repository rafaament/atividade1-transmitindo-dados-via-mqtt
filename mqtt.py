import os 

import paho.mqtt.client as mqtt

# Configurações do broker MQTT
MQTT_BROKER = os.getenv('MQTT_BROKER')
MQTT_PORT = 1883
MQTT_TOPIC = "temperatura/cpu"

# Função de callback para quando uma mensagem é recebida
def on_message(client, userdata, msg):
    print(f"Temperatura recebida via MQTT: {msg.payload.decode()} C")

# Configurar cliente MQTT
client = mqtt.Client()
client.on_message = on_message

# Conectar ao broker MQTT
client.connect(MQTT_BROKER, MQTT_PORT, 60)

# Inscrever-se no tópico
client.subscribe(MQTT_TOPIC)

# Loop de recepção de mensagens
client.loop_forever()