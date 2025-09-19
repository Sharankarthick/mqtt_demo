import paho.mqtt.client as mqtt
import firebase_admin
from firebase_admin import credentials, db
import json
import time

# --- Firebase Configuration ---
cred = credentials.Certificate("serviceAccountKey.json") # <-- DOWNLOAD BY 'GENERATING NEW PRIVATE KEY' FROM PROJECT SETTINGS UNDER SERVICE ACCOUNTS 
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://test-2k23-default-rtdb.firebaseio.com/' # <-- PASTE YOUR DATABASE URL HERE
})

# --- HiveMQ Configuration ---
HIVEMQ_HOST = '0d83ce06dfce44298b85b65b560bbd19.s1.eu.hivemq.cloud' # <-- PASTE YOUR BROKER URL HERE
HIVEMQ_PORT = 8883
HIVEMQ_USER = 'mqttDemo' # <-- PASTE YOUR USERNAME HERE
HIVEMQ_PASS = 'Mqtt1234' # <-- PASTE YOUR PASSWORD HERE

# --- MQTT Topics & Firebase Paths ---
MQTT_SENSOR_TOPIC = 'arduino/sensor'
FIREBASE_LIVE_DATA_PATH = 'arduino/live_data'
FIREBASE_CONTROL_PATH = 'arduino/control'

# --- MQTT Client Callbacks ---
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected successfully to HiveMQ Broker!")
        # Subscribe to the Arduino's sensor data topic
        client.subscribe(MQTT_SENSOR_TOPIC)
        print(f"Subscribed to topic: {MQTT_SENSOR_TOPIC}")
    else:
        print(f"Failed to connect, return code {rc}\n")

def on_message(client, userdata, msg):
    """
    Called when a message is received from the MQTT broker.
    It pushes the data to the Firebase 'live_data' path.
    """
    print(f"Received from MQTT topic `{msg.topic}`: {msg.payload.decode()}")
    try:
        payload_dict = json.loads(msg.payload.decode())
        db.reference(FIREBASE_LIVE_DATA_PATH).set(payload_dict)
        print("Pushed sensor data to Firebase.")
    except json.JSONDecodeError:
        print("Error decoding JSON from MQTT payload.")
    except Exception as e:
        print(f"An error occurred: {e}")

# --- Firebase Listener ---
def firebase_listener(event):
    """
    Called when data changes at the 'control' path in Firebase.
    It publishes this command to the appropriate MQTT topic.
    """
    print("Change detected in Firebase 'control' path.")
    if event.data and isinstance(event.data, dict):
        try:
            topic = event.data.get("topic")
            payload = event.data.get("payload")

            if topic and payload is not None:
                print(f"Publishing to MQTT topic `{topic}`: {payload}")
                client.publish(topic, str(payload))
            else:
                print("Invalid command format from Firebase.")
        except Exception as e:
            print(f"Error processing Firebase event: {e}")

# --- Main Script ---
# Initialize MQTT Client
client = mqtt.Client(protocol=mqtt.MQTTv311)
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(HIVEMQ_USER, HIVEMQ_PASS)
client.tls_set()
client.connect(HIVEMQ_HOST, HIVEMQ_PORT, 60)

# Start the Firebase listener in the background
db.reference(FIREBASE_CONTROL_PATH).listen(firebase_listener)
print(f"Firebase listener started on path: {FIREBASE_CONTROL_PATH}")

# Start the MQTT client loop (this is a blocking call)
print("Starting MQTT client loop...")
client.loop_forever()
