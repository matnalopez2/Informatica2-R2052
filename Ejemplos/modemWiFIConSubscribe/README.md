
## Instalar Driver
Para instalar el driver:
* Descargar de: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads
* Instalar el driver:
    * Abrir Administrador de dispositivos 
    * -> Puertos (COM y LPT) 
    * -> Instalar driver 
    * -> Elegir carpeta CP210x_Universal_Windows_Driver 
    * -> Siguiente -> Finalizar

## Configurar IDE para cargar el código al microcontrolador
Con el driver instalado, abrir Arduino IDE
* Elegir placa "NodeMCU 1.0 (ESP-12E Module)"
* Elegir Puerto COM que se vea en el Administrador de dispositivos y que incluya CP210x en el nombre

## Instalar librerías

Para poder utilizar este código, se necesita instalar las siguientes librerías:
* PubSubClient by Nick O'Leary - versión 2.8.0 (latest)

## Editar código según tu configuración
Para poder utilizar el script correctamente se deben definir los siguientes parámetros:
* ssid: Este es el nombre de la red wifi donde se conectará el dispositivo
* password: Esta es la contraseña de la red wifi configurada anteriormente
* mqtt_server: Aquí se aloja el servidor MQTT que recibirá los datos de tu script. Puedes utilizar el que está por defecto y puedes configurar tanto una IP (privada o pública) como un nombre de dominio.
* mqtt_port: Aquí se configura el puerto en el que escucha el servidor MQTT. Utiliza 1883 para una conexión insegura o 8883 para una conexión mediante SSL. (Asegúrate de que el servidor se encuentre configurado de forma acorde)
* clientID: Este será el identificador de tu dispositivo.
* mqtt_username: Este será el nombre de usuario que debes definir en el servidor MQTT que utilices. Si utilizas el servidor por defecto, desestima este parámetro.
* mqtt_password: Esta será la contraseña del usuario que debes definir en el servidor MQTT que utilices. Si utilizas el servidor por defecto, desestima este parámetro.
* topic_enviar: Este es el parámetro que define el tópico o canal por el cual estarás escribiendo datos en el servidor MQTT.
* topic_recibir: Este es el parámetro que define el tópico o canal por el cual estarás recibiendo datos desde el servidor MQTT.


## Subir el código al microcontrolador
Ahora podemos Compilar y luego Subir el archivo a nuestro ESP. Una vez subido al microcontrolador, abrimos el Monitor Serie



