#include <Key.h>
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
// Reemplazar con sus datos de su red WIFI
const char* ssid = "AP_asix";
const char* password = "AP_asix2023";
const char* mqttServer = "10.0.110.103";
const int mqttPort = 1883;
const char* mqttUser = "sensor"; //sensor o control
const char* mqttPassword = "sensor";

//********* Nom del client MQTT
const char* clientID = "Patata";
//********* TOPICS
const char* topicCTR = "fabrica/#";
const char* topicALARMA = "fabrica/alarma";
const char* topicTEMP = "fabrica/pass";
const char* topicHUM = "fabrica/forbiden";
const char* alarma = "ALARMA!";
// Inicializar BOT Telegram
//#define BOTtoken "AAGxenTw2DSrE4iXz4dWVAbd8ITWQ25vryM"  // Tu Bot Token (Obtener de Botfather)
//6877210011:
// Usa @myidbot para averiguar el chat ID
// Tambien necesita hacer click en "start" antes de enviarle mensajes el bot
//#define CHAT_ID "-4011925306"

WiFiClientSecure client;
//UniversalTelegramBot bot(BOTtoken, client);
#define ROW_NUM     4 // cuatro filas
#define COLUMN_NUM  4 // cuatro columnas
#define DHTPIN 27    //D27 del ESP27 Dev Module
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//********* PANTALLA OLED
#define oled 32
Adafruit_SSD1306 display = Adafruit_SSD1306(128, oled, &Wire);

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM]      = {19, 18, 5, 17}; // GPIO19, GPIO18, GPIO5, GPIO17 conectados a los pines de las filas
byte pin_column[COLUMN_NUM] = {16, 4, 0, 2};   // GPIO16, GPIO4, GPIO0, GPIO2 conectados a los pines de las columnas

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

int storedNumber = 19921124; // Número previamente almacenado para comparación
int userInput = 0; // Variable para almacenar el número introducido por el usuario
int contador = 0;

 void setup() {
  Serial.begin(9600);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert("1132582343"); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


   dht.begin();
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
   display.display();
   display.clearDisplay();
   display.display();
   display.setTextSize(0.5);
   display.setTextColor(WHITE,BLACK);
 }

void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  Serial.print("Missatge rebut del topic [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    messageTemp+=(char)payload[i];
  }
  Serial.println();
//***************** topicALARMA **********
//*************************************
  if (String(topic) == topicALARMA) {
    if(messageTemp == "alarma"){
      display.clearDisplay();
      display.setCursor(10, 0);
      display.print("ALARMA!");
      display.display();
      delay(500);
      
      display.clearDisplay();
    }
    if(messageTemp == "alarmaTOP"){
      Serial.println("ALARMA");
      // PRINTEM A LA OLED
      for (int i=0;i<10;i++){
        display.clearDisplay();
        display.setCursor(10, 0);
        display.print("ALARMA!");
        display.display();
        delay(200);
        display.clearDisplay();
        
        display.display();
        delay(200);
        display.clearDisplay();
      }
    }
   
    else if(messageTemp == "off"){
      Serial.println("off");
      display.clearDisplay();
    }
    
    }
  

//***************** topic HUMITAT **********
//*************************************
  if (String(topic).indexOf("password") > 0) {
    //PRINTAR A LA OLED
    if (String(topic) == "fabrica/pass"){
      //display.clearDisplay();
      display.setCursor(0, 0);
      display.print("R1: ");    
      display.print(messageTemp);
      display.println("%");
      display.display();
    }
    else if ((String(topic) == "fabrica/forbidden")){
      //display.clearDisplay();
      display.setCursor(0, 32);
      display.print("R2: ");     
      display.print(messageTemp);
      display.println("%");
      display.display();
      client.publish(topicALARMA,alarma);
    }
  } 
    
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
      if (client.subscribe(topicCTR)){
        Serial.print("subscrit correctament al topic ");
        Serial.println(topicCTR);
      }else{
        Serial.print("ERROR al subscriures al topic ");
        Serial.println(topicCTR);
      }
      if (client.subscribe(topicALARMA)){
        Serial.print("subscrit correctament al topic ");
        Serial.println(topicALARMA);
      }else{
        Serial.print("ERROR al subscriures al topic ");
        Serial.println(topicALARMA);
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
  //indiquem els pins dels LEDs
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledY, OUTPUT);
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
  display.setTextSize(2);
 	display.setTextColor(WHITE,BLACK);
}

void loop() {
  
  //guardem els valors del sensor
  float humitat = dht.readHumidity();
  float temperatura = dht.readTemperature();
  
  //Si no estem connectats, ens reconnectem
  if (!client.connected()) {
    reconnect();
  } 
  //*********** PUBLICAR DADES ********
  //***********************************
  if(client.connected()){
    if(mqttUser=="sensor"){
      //mostrem els valors del sensor pel monitor série
      Serial.print(humitat, 1);
      Serial.print("\t\t");
      Serial.print(temperatura, 1);
      Serial.println("\t\t");
      //Preparem format dades per publicar
      char humitatChar[6];
      dtostrf(humitat,5,1,humitatChar); //Conversió a char
      client.publish(topicHUM,humitatChar); //enviem topicHUM
      char temperaturaChar[6];
      dtostrf(temperatura,5,1,temperaturaChar); //Conversió a char
      client.publish(topicTEMP,temperaturaChar); //enviem topicTEMP
      //Printem a la pantalla OLED
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Bienvenidos a ");
      //display.println(temperaturaChar);
      display.print("INFOHUB 😃😃😃😃");
      //display.print(humitatChar);
      //display.print("%");
      display.display();
    }
    //delay(500);
    if(mqttUser == "control"){
      delay(50);
    }else{
      delay(500);
    }
  }
  client.loop();
}
void loop() {
  char key = keypad.getKey(); // Obtener la tecla presionada por el usuario
  display.setCursor(0, 0);

  if (key) {
    if (key >= '0' && key <= '9') {
      display.println(key);
      
      display.display();
      Serial.print("*"); // Imprimir la tecla presionada (se cambió println por print)
      userInput = userInput * 10 + (key - '0'); // Construir el número introducido por el usuario
      contador = contador + 1;
    } 
    else {
      display.print("\nLas letras y los simbolos no son validos");
      display.display();
      display.clearDisplay();
      Serial.println("\nLas letras y los simbolos no son validos"); // Imprimir mensaje de tecla no válida
    }
  }
  
  if (contador == 8) {
    if (userInput == storedNumber) {
      display.print("\n¡Contrasenya correcta!");
      display.display();
      display.clearDisplay();
      Serial.println("\n¡Contraseña correcta!"); // El número introducido es correcto
    } 
    else {
      
      display.print("\nContrasenya incorrecta, intentalo otra vez");
      display.display();
      Serial.println("\nContraseña incorrecta, intentalo otra vez"); // El número introducido es incorrecto
      display.clearDisplay();
    }
    userInput = 0; // Restablecer el valor de userInput para la próxima entrada
    contador = 0; // Reiniciar el contador
  }
}
