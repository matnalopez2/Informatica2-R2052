/*
  Basic ESP8266 MQTT example

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Elegí valores acordes a tu red.
const char* ssid          = "Mati";                  // Nombre de tu red wifi
const char* password      = "0038464295";              // Contraseña de tu red wifi

//const char* mqtt_server = "192.168.0.100";                    // IP donde corre el broker MQTT.
//const char* mqtt_server   = "mqtt.matnalopez.com.ar";           // Dominio donde corre el broker MQTT.
const char* mqtt_server   = "186.22.239.145";                   // IP Pública donde corre el broker MQTT.
const int   mqtt_port     = 1883;                               // Puerto donde corre el broker MQTT. 1883 inseguro - 8883 con SSL 

const char* clientID      = "NODEMCU";
const char* mqtt_username = "esp";
const char* mqtt_password = "8266";

const char* topic = "canal";

// Inicializar los objetos WiFi y Cliente MQTT
WiFiClient wifiClient;
PubSubClient client(mqtt_server, mqtt_port, wifiClient);

long lastMsg = 0;
char msg[50];
long int value = 0;
float corriente=0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  Serial.println("esp-start");
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Debugging - Output the IP Address of the ESP8266
  Serial.print("Conectando a la siguiente red: ");
  Serial.println(ssid);
  //client.setCallback(callback);
  
  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    client.subscribe(topic);
    Serial.println("Conectado al Broker MQTT!");
  }
  else {
    Serial.println("Conección al Broker MQTT fallida...");
  }
}

bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
      client.subscribe(topic);
      return true;
    }
    else {
      return false;
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Conectando a la siguiente red: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("---------------");
  Serial.print("WiFi conectado, con IP: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Conectado al Broker MQTT!");
    client.publish(topic, "Esp8266 conectado correctamente");
  }
  else {
    Serial.println("Conección al Broker MQTT fallida...");
  }
}

void reconnect() {
  // Loop until we're reconnected
  delay(1000);
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Esp8266")) {
      Serial.println("Conectado");
      client.subscribe(topic);
    } else {  
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 5 segundos...");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  String readSerialMsg;

  if (!client.connected()) {
      client.publish(topic, "Esp8266 se ha desconectado");
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    readSerialMsg = readSerial();
    readSerialMsg.toCharArray(msg,readSerialMsg.length()+1);     
    if (isNotEmpty(msg)) {
    Serial.print("Publish message: ");
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(msg);
      digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
      client.publish(topic, msg);
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    } else { 
      // Hago algo si el mensaje está vacío
    }
  }
}

boolean isNotEmpty(String data) {
  return data != "";
}

String readSerial() {
  String serialData = "";
  if (Serial.available()) {
    serialData = Serial.readStringUntil('\r\n');
  }
  return serialData;
}
