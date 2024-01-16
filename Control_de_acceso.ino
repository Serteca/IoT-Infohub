#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// Definir los detalles de la red WiFi
const char* ssid = "AP_asix";
const char* password = "AP_asix2023";

// Definir los detalles del servidor MQTT
const char* mqtt_server = "10.0.110.103";
const int mqtt_port = 1883;
const char* mqtt_user = "sensor";
const char* mqtt_password = "sensor";
const char* topicACE = "resultado/control_acceso";

WiFiClient espClient;
PubSubClient client(espClient);

// Definir la configuración del teclado matricial y los pines a los que está conectado
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 14, 27}; // Conectar a los pines del ESP32
byte colPins[COLS] = {15, 2, 0}; // Conectar a los pines del ESP32

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Inicializar la pantalla
#define SCREEN_WIDTH 128 // Ancho de la pantalla OLED, en píxeles
#define SCREEN_HEIGHT 64 // Altura de la pantalla OLED, en píxeles
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Manejar los mensajes recibidos por MQTT si es necesario
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqtt_user, mqtt_password )) {
      // Suscribirse a los temas si es necesario
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    // Mostrar la tecla presionada en el monitor serial
    // Realizar la lógica de validación del código de acceso con el teclado matricial
    // Enviar el resultado por MQTT al otro dispositivo
    if (client.connected()) {
      client.publish(topicACE, "Acceso Permitido o Denegado");  
    }
  }
  client.loop();
  if (!client.connected()) {
    reconnect();
  }
}
