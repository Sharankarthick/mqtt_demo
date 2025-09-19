#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Spidy";
const char* password = "skwgpcgr";

// HiveMQ credentials
const char* mqtt_server = "0d83ce06dfce44298b85b65b560bbd19.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "mqttDemo";
const char* mqtt_password = "Mqtt1234";

// MQTT topics
const char* topic_sensor = "arduino/sensor";
const char* topic_motor = "arduino/motor";

// Hardware pins
const int SENSOR_PIN = 2;
const int MOTOR_PIN = 9;


WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastMsg = 0;
int sensorValue = 0;
int motorSpeed = 0;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
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

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  // Control LED based on received message
  if (String(topic) == topic_motor) {
    // Convert the message to an integer
    int pwmValue = message.toInt();
    
    // Constrain the value to the valid PWM range (0-255)
    pwmValue = constrain(pwmValue, 0, 255);
    
    Serial.print("Setting motor speed to: ");
    Serial.println(pwmValue);
    
    // Use analogWrite to set the motor speed
    analogWrite(MOTOR_PIN, pwmValue);
    motorSpeed = pwmValue; // Update our state variable
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ArduinoClient-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic_motor);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) { // Send data every 2 seconds
    lastMsg = now;
    
    // Read sensor value
    sensorValue = digitalRead(SENSOR_PIN);
    
    // Create JSON payload
    String payload = "{\"sensor\":";
    payload += sensorValue;
    payload += ",\"motor_speed\":";
    payload += motorSpeed;
    payload += ",\"timestamp\":";
    payload += now;
    payload += "}";
    
    // Publish to MQTT
    client.publish(topic_sensor, payload.c_str());
    
    Serial.print("Published: ");
    Serial.println(payload);
  }
}
