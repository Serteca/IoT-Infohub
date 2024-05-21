#include <SoftwareSerial.h>
#include <SPI.h>
#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define ROW_NUM     4 // Cuatro filas
#define COLUMN_NUM  4 // Cuatro columnas

#define ESP32_RX 16  // Cambiar al pin RX correcto de la ESP32
#define ESP32_TX 17  // Cambiar al pin TX correcto de la ESP32
SoftwareSerial mySerial(ESP32_RX, ESP32_TX);

static int8_t Send_buf[8] = {0};

#define CMD_PLAY_W_INDEX 0X03
#define CMD_SET_VOLUME 0X06
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY 0X0D
#define CMD_NEXT 0X01 // Nuevo comando agregado para reproducir la siguiente canción
#define CMD_PLAY_W_VOL 0X22

int currentSongIndex = 0; // Índice de la canción actual

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
int alarma=0;
/***************************************************** 
*****    PARÀMETRES A MODIFICAR   ********************
******************************************************/
//********* Connexió WiFi
const char* ssid = "AP_asix";
const char* password =  "AP_asix2023";
//********* Connexió MQTT
const char* mqttServer = "10.0.110.111";
const int mqttPort = 1883;
const char* mqttUser = "ADMIN"; //sensor o control
const char* mqttPassword = "ASIX";

//********* Nom del client MQTT
const char* clientID = "Patata";

//********* PANTALLA OLED
#define oled 64


Adafruit_SSD1306 display = Adafruit_SSD1306(128, oled, &Wire);
static const unsigned char PROGMEM imgNuclear[816] = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 
  0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0xC0, 
  0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 
  0x60, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x60, 0x00, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 
  0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 
  0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 
  0x00, 0x00, 0x00, 0xF0, 0xFF, 0x01, 0x60, 0x00, 0xF8, 0xFF, 0x00, 0x00, 
  0x00, 0xF0, 0xFF, 0x03, 0x60, 0x00, 0xFC, 0xFF, 0x00, 0x00, 0x00, 0xF0, 
  0xC3, 0x07, 0x60, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0x30, 0x00, 0x0E, 
  0x60, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x1C, 0x60, 0x80, 
  0x03, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x38, 0x60, 0xC0, 0x01, 0xC0, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0x70, 0x60, 0x60, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x30, 0x00, 0xE0, 0x60, 0x38, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0xC0, 0xFF, 0x1F, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x80, 
  0x01, 0x18, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x80, 0x00, 0x10, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x10, 0x00, 0xC0, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0xC0, 0x00, 0x10, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x70, 0x00, 0xC0, 0x00, 0x10, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xF0, 
  0xFF, 0xFF, 0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x70, 0x00, 0xC0, 
  0x00, 0x10, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x80, 0x00, 0x10, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x80, 0x00, 0x10, 0x00, 0xC0, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0x80, 0x00, 0x10, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x30, 0x00, 0x80, 0x00, 0x18, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x80, 0xFF, 0x1F, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0xC0, 
  0xFF, 0x3F, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0xE0, 0x60, 0x70, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x70, 0x60, 0xE0, 0x00, 0xC0, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x60, 0xC0, 0x01, 0xC0, 0x00, 0x00, 
  0x00, 0x30, 0x00, 0x1C, 0x60, 0x80, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x0E, 0x60, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0xF0, 0x9F, 0x03, 
  0x60, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0x01, 0x60, 0x00, 
  0xFC, 0xFF, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x60, 0x00, 0x00, 0xE0, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 
  0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 
  0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 
  0x00, 0xC0, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 
  0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 
  0x00, 0x70, 0x00, 0x00, 0x60, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xE0, 
  0x00, 0x00, 0x60, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 
  0xFC, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

byte pin_rows[ROW_NUM]      = {19, 18, 5, 27}; // GPIO19, GPIO18, GPIO5, GPIO27 conectados a los pines de las filas
byte pin_column[COLUMN_NUM] = {26, 4, 0, 2};   // GPIO26, GPIO4, GPIO0, GPIO2 conectados a los pines de las columnas

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

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

void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e;
  Send_buf[1] = 0xff;
  Send_buf[2] = 0x06;
  Send_buf[3] = command;
  Send_buf[4] = 0x00; // No feedback
  Send_buf[5] = (int8_t)(dat >> 8);
  Send_buf[6] = (int8_t)(dat);
  Send_buf[7] = 0xef;
  for (uint8_t i = 0; i < 8; i++)
  {
    mySerial.write(Send_buf[i]);
  }
}

#define MP3_TX_PIN 15 // Pin TX del módulo serial MP3
#define MP3_RX_PIN 13 // Pin RX del módulo serial MP3
 
void setup() {
  //inicialitzem el port serie
  Serial.begin(9600);
  //pins mp3
  pinMode(MP3_TX_PIN, OUTPUT);
  pinMode(MP3_RX_PIN, INPUT);
  
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
  reconnect();
  
  //rutina de gestió de dades rebudes
  client.setCallback(callback);
  //inicialitzem la pantalla LCD
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();
  display.clearDisplay();
 	display.display();
  display.setTextSize(1);
 	display.setTextColor(WHITE,BLACK);
  display.drawBitmap(11, 3, imgNuclear, 76, 64, 1);
  display.display();
  delay(10000);
  display.clearDisplay();
 	display.display();
  display.print("Connectant ...");
  display.display();
  delay(2000);
  display.clearDisplay();
 	display.display();
  display.print("Connectat!   ");
  display.display();
  delay(1000);
  display.clearDisplay();
 	display.display();
  mySerial.begin(9600);
  delay(500);
  sendCommand(CMD_SEL_DEV, DEV_TF);
  delay(200);
  
  }

void loop() {
 char key = keypad.getKey();
 display.setCursor(0, 0);
 display.println("Escriu el PIN:\n");
 display.display();
 if (key) {
   if (key >= '0' && key <= '9') {
     //display.print(key);
     client.publish(MQTT_TOPIC, "*");
     display.display();
     Serial.print("*");
     userInput = userInput * 10 + (key - '0');
     display.print(userInput);
     display.display();
     characterCount++;

   } else {
     display.println("\nCaracters invalids");
     display.display();
     delay(1500);
     display.clearDisplay();
     Serial.println("\nCaracters invàlids");
     display.println("\nCaracters invàlids");
     display.display();
   }
 }

 if (characterCount == 8) {
   if (userInput != storedPassword) {
    if (contador_fallos < 3){

     display.println("\nContrasenya incorrecta,\nintenta-ho de nou");
     display.display();
     Serial.println("\nContrasenya incorrecta,\nintenta-ho de nou");
     delay(1500);
     display.clearDisplay();
     contador_fallos++;
     userInput = 0; // Restablecer el valor de userInput para la próxima entrada
     characterCount = 0;
     // Publicar mensaje MQTT de acceso denegado
     client.publish(MQTT_TOPIC, "Accés Denegat");
   }
    else{
    
     display.println("\nALARMA!");
     display.display();
     Serial.println("\nS'ha arribat al límit d'alarmes");
     display.display();
     delay(500);
     display.clearDisplay();
     userInput = 0; // Restablecer el valor de userInput para la próxima entrada
     characterCount = 0;
     // Publicar mensaje MQTT de acceso denegado
     client.publish(MQTT_TOPIC, "¡ALARMA!");
     client.publish(MQTT_TOPIC, "\nS'ha arribat al límit d'errors");
     //playAudio("mi_archivo.mp3");
     if (alarma < 3){
     currentSongIndex++;
     sendCommand(CMD_PLAY_W_INDEX, currentSongIndex); // Reproducir la primera canción
     alarma++;
     }
     else{
          Serial.println("\nS'ha arribat al límit d'alarmes");
          display.println("ACCES NO\nAUTORITZAT");
          display.display();
          delay(60000);
          display.clearDisplay();
 	        display.display();
          alarma=0; 
     }
     
    }     
   }
   else {
    Serial.println("\nContrasenya correcta!");
    display.clearDisplay();
    display.display();
      // Publicar mensaje MQTT de acceso concedido
    client.publish(MQTT_TOPIC, "Accés Concedit");
    display.print(gpass);
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
    contador_fallos++;
    userInput = 0; // Restablecer el valor de userInput para la próxima entrada
    characterCount = 0; // Reiniciar el contador de caracteres
  }
}
}