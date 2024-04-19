/** CODI ESP32 PER ENVIAR TEMPERATURA I HUMITAT DEL SENSOR DHT11
** A UN BROKER MQTT. TAMBÉ ES SUBSCRIU A UN CANAL PER ON REBRÀ
** SENYALS D'ALARMA QUE ENCENDRAN UN LED 
**/
//**** LLIBRERIES PER PANTALLA
// ESP32 NodeMCU I2C: SCL GPIO22, SDA GPIO21
#include <SPI.h>
#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
//#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define ROW_NUM     4 // Cuatro filas
#define COLUMN_NUM  4 // Cuatro columnas

char keys[ROW_NUM][COLUMN_NUM] = {
 {'1', '2', '3', 'A'},
 {'4', '5', '6', 'B'},
 {'7', '8', '9', 'C'},
 {'*', '0', '#', 'D'}
};

const char* MQTT_TOPIC = "access-controlinfohub";
const char* gpass = "\n¡Contraseña correcta!";
int storedPassword = 19921124; // Contraseña previamente almacenada para comparación
int userInput = 0; // Variable para almacenar el número introducido por el usuario
int characterCount = 0;
int contador_fallos = 0;

/***************************************************** 
*****    PARÀMETRES A MODIFICAR   ********************
******************************************************/
//********* Connexió WiFi
const char* ssid = "AP_asix";
const char* password =  "AP_asix2023";
//********* Connexió MQTT
const char* mqttServer = "10.0.110.113";
const int mqttPort = 1883;
const char* mqttUser = "sensor"; //sensor o control
const char* mqttPassword = "sensor";

//********* Nom del client MQTT
const char* clientID = "Patata";

//********* PANTALLA OLED
#define oled 32


Adafruit_SSD1306 display = Adafruit_SSD1306(128, oled, &Wire);

byte pin_rows[ROW_NUM]      = {19, 18, 5, 17}; // GPIO19, GPIO18, GPIO5, GPIO17 conectados a los pines de las filas
byte pin_column[COLUMN_NUM] = {16, 4, 0, 2};   // GPIO16, GPIO4, GPIO0, GPIO2 conectados a los pines de las columnas

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

#define DHTPIN 27    //D27 del ESP27 Dev Module
  #define DHTTYPE DHT11 
  DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);


//Rutina de gestió de dades quan rebem un TOPIC
void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  Serial.print("Missatge rebut del topic ");
  Serial.print(topic);
  Serial.print(" .");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    messageTemp+=(char)payload[i];
  }
  Serial.println();

}

void reconnect() {
  // Loop fins que es reconecta
  while (!client.connected()) {
    Serial.print("Intentant connectar-se al servidor MQTT ");
    Serial.println(mqttServer);
   if (client.connect(clientID, mqttUser, mqttPassword )) {
      Serial.println("connectat!");
      //******* subscripció al TOPICS ****
      //**********************************
      if (client.subscribe(MQTT_TOPIC)){
        Serial.print("subscrit correctament al topic ");
        Serial.println(MQTT_TOPIC);
      }else{
        Serial.print("ERROR al subscriures al topic ");
        Serial.println(MQTT_TOPIC);
      }
      
    } else {
      Serial.print("fallida, rc=");
      Serial.print(client.state());
      Serial.println(" tornarà a intentar-se en 5 segons");
      // Esperar 5 segons i reintentar
      delay(5000);
    }
  }
}
 
void setup() {
  //inicialitzem el port serie
  Serial.begin(115200);
  //connexió WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connectant-se al WiFi: ");
    Serial.println(ssid);
  }
  Serial.println("Connectat al WiFi!");
  //CERTIFICATS
  //espClient.setCACert(CA_cert); 
  //espClient.setCertificate(ESP_CA_cert); 
  //espClient.setPrivateKey(ESP_RSA_key);  
  //indiquem el servidor MQTT
  client.setServer(mqttServer, mqttPort);
  //rutina de gestió de dades rebudes
  client.setCallback(callback);
  //inicialitzem el sensor
  dht.begin();
  //inicialitzem la pantalla LCD
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();
  display.clearDisplay();
 	display.display();
  display.setTextSize(1.5);
 	display.setTextColor(WHITE,BLACK);
}

void loop() {
 char key = keypad.getKey();
 display.setCursor(0, 0);
 display.println("Escribe el PIN:\n");
 display.display();
 if (key) {
   if (key >= '0' && key <= '9') {
     //display.print(key);
     display.display();
     client.publish(MQTT_TOPIC, "*");
     Serial.print("*");
     userInput = userInput * 10 + (key - '0');
     display.print(userInput);
     display.display();
     characterCount++;

   } else {
     display.println("\nCaracteres inválidos");
     display.display();
     delay(1500);
     display.clearDisplay();
     Serial.println("\nCaracteres inválidos");
     display.println("\nCaracteres inválidos");
     display.display();
   }
 }

 if (characterCount == 8) {
   if (userInput != storedPassword) {
      if (contador_fallos <= 3){
     display.println("\nContraseña incorrecta, intenta de nuevo");
     display.display();
     Serial.println("\nContraseña incorrecta, intenta de nuevo");
     delay(1500);
     display.clearDisplay();
     // Publicar mensaje MQTT de acceso denegado
     client.publish(MQTT_TOPIC, "Acceso Denegado");
   }
    else{
     display.println("\n¡ALARMA!");
     display.display();
     Serial.println("\nSe ha alcanzado el límite de fallos de contraseña");
     display.display();
     delay(500);
     display.clearDisplay();
     // Publicar mensaje MQTT de acceso denegado
     client.publish(MQTT_TOPIC, "¡ALARMA!");
     client.publish(MQTT_TOPIC, "Se ha alcanzado el límite de fallos de contraseña");
    }     
   }
   else {
    Serial.println("\n¡Contraseña correcta!");
      // Publicar mensaje MQTT de acceso concedido
    client.publish(MQTT_TOPIC, "Acceso Concedido");
    display.print(gpass);
    display.display();
    display.clearDisplay();
    display.display();
    contador_fallos++;
    userInput = 0; // Restablecer el valor de userInput para la próxima entrada
    characterCount = 0; // Reiniciar el contador de caracteres
  }
}
}