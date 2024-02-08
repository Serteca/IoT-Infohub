#include <Key.h>
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>

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
   dht.begin();
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
   display.display();
   display.clearDisplay();
   display.display();
   display.setTextSize(0.5);
   display.setTextColor(WHITE,BLACK);
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
      delay(1000);
      display.clearDisplay();
      Serial.println("\nLas letras y los simbolos no son validos"); // Imprimir mensaje de tecla no válida
    }
  }
  
  if (contador == 8) {
    if (userInput == storedNumber) {
      
      display.print("\n¡Contrasenya correcta!");
      display.display();
      delay(1000);
      display.clearDisplay();
      Serial.println("\n¡Contraseña correcta!"); // El número introducido es correcto
    } 
    else {
      
      display.print("\nContrasenya incorrecta, intentalo otra vez");
      display.display();
      
      Serial.println("\nContraseña incorrecta, intentalo otra vez"); // El número introducido es incorrecto
      delay(1000);
      display.clearDisplay();
    }
    userInput = 0; // Restablecer el valor de userInput para la próxima entrada
    contador = 0; // Reiniciar el contador
  }
}
