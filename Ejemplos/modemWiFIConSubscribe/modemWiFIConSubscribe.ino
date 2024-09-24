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
const char* ssid          = "NOMBRE_RED_WIFI";                  // Nombre de tu red wifi
const char* password      = "CONTRASEÑA_RED_WIFI";              // Contraseña de tu red wifi

// Aquí definiremos los parámetros que permiten conectarnos al servidor MQTT remoto. Debemos definir su IP o nombre de dominio (sólo uno) y su puerto.
//const char* mqtt_server = "192.168.0.100";                    // IP donde corre el broker MQTT. Debes reemplazar esta IP por la IP privada de tu red LAN.
const char* mqtt_server   = "mqtt.matnalopez.com.ar";           // Dominio donde corre el broker MQTT. Puedes reemplazar este dominio por el tuyo, alguno online gratis o utilizar este por defecto.
const int   mqtt_port     = 1883;                               // Puerto donde corre el broker MQTT. (1883 inseguro - 8883 con SSL)
const char* clientID      = "NODEMCU";                          // Debes definir un clientID que haga sentido para tu proyecto y tu dispositivo.
const char* mqtt_username = "esp";                              // Debes definir un usuario que haga sentido para tu proyecto y tus configuraciones de seguridad.
const char* mqtt_password = "8266";                             // Debes definir una contraseña que haga sentido para tu proyecto y tus configuraciones de seguridad.

const char* topic_enviar  = "micro-a-server";                   // Debes definir un topic que haga sentido para tu proyecto.
const char* topic_recibir = "server-a-micro";                   // Debes definir un topic que haga sentido para tu proyecto.


// Inicializar los objetos WiFi y Cliente MQTT
WiFiClient wifiClient;
PubSubClient client(mqtt_server, mqtt_port, wifiClient);

long lastMsg = 0;
char msg[50];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);                                 // Inicializamos el pin BUILTIN_LED como salida
  Serial.begin(9600);                                           // Configuramos el puerto serie en 9600 bps
  setup_wifi();                                                 // Inicializamos el módulo WIFI
  client.setServer(mqtt_server, mqtt_port);                     // Configuramos el servidor MQTT, definiendo IP/URL y puerto

  // Debugging - Output the IP Address of the ESP8266
  Serial.print("Conectando a la siguiente red: ");              // Output de debug
  Serial.println(ssid);

  // Agregamos la función de callback cuando queremos recibir info mediante MQTT hacia el microcontrolador
  client.setCallback(callback_function);
  
  // Conexión al Broker MQTT. Si falla, revisar que usuario y contraseña sean válidos en el broker
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    client.subscribe(topic_recibir);                            // Suscripción al topic_recibir
    Serial.println("Conectado al Broker MQTT!");                // Output de debug
  }
  else {
    Serial.println("Conección al Broker MQTT fallida...");      // Output de debug
  }
}

void callback_function(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");                            // Output de debug
  Serial.print(topic);                                          // Output de debug
  Serial.print("] ");                                           // Output de debug
  for (int i=0;i<length;i++) {        
    Serial.print((char)payload[i]);                             // Output de debug
  }
  Serial.println();                                             // Output de debug
}

bool Connect() {
  // Conectarse al servidor MQTT y suscribirse al topic_recibir
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
      client.subscribe(topic_recibir);                          // Suscripción al topic_recibir
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
  Serial.print("Conectando a la siguiente red: ");              // Output de debug
  Serial.println(ssid);                                         // Output de debug

  WiFi.begin(ssid, password);                                   // Establecer conexión WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");                                          // Output de debug
  }

  Serial.println("---------------");                            // Output de debug
  Serial.print("WiFi conectado, con IP: ");                     // Output de debug
  Serial.println(WiFi.localIP());                               // Output de debug

  // Conexión al Broker MQTT. Si falla, revisar que usuario y contraseña sean válidos en el broker
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Conectado al Broker MQTT!");                // Output de debug
    client.publish(topic_enviar, "Esp8266 conectado");          // Publicar en topic_enviar de debug
  }
  else {
    Serial.println("Conección al Broker MQTT fallida...");      // Output de debug
  }
}

void reconnect() {
  // Loop hasta que logremos reconectar
  delay(1000);
  while (!client.connected()) {
    // Intento de conexión
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("Conectado");                              // Output de debug
      client.subscribe(topic_recibir);                          // Suscripción al topic_recibir
    } else {  
      Serial.print("Falló, error número =");                    // Output de debug
      Serial.print(client.state());                             // Output de debug
      Serial.println(" reintentando en 5 segundos...");         // Output de debug                 
      delay(5000);                                              // Esperar 5 segundos antes de reintentar
    }
  }
}

void loop() {

  String readSerialMsg;

  if (!client.connected()) {
      client.publish(topic_enviar, "Esp8266 desconectado");
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    readSerialMsg = readSerial();                               // Leo el mensaje que llegó por el puerto serie
    readSerialMsg.toCharArray(msg,readSerialMsg.length()+1);    // Transformo el mensaje en un array de chars
    if (isNotEmpty(msg)) {                                      // Validamos que haya llegado un msg y no esté vacío.
    Serial.print("Publish message: ");                          // Output de debug
    Serial.print(topic_enviar);                                 // Output de debug
    Serial.print(" -> ");                                       // Output de debug
    Serial.println(msg);                                        // Output de debug
      digitalWrite(LED_BUILTIN, LOW);                           // Turn the LED on (Note that LOW is the voltage level
      client.publish(topic_enviar, msg);                        // Publico en el topic_enviar el msg que recibí por el puerto serie
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);                          // Turn the LED off by making the voltage HIGH
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
