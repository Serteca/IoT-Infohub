#include <Keypad.h>

#define ROW_NUM     4 // cuatro filas
#define COLUMN_NUM  4 // cuatro columnas

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
  Serial.begin(9600); // Iniciar la comunicación con el puerto serie
}

void loop() {
  char key = keypad.getKey(); // Obtener la tecla presionada por el usuario
  
  if (key) {
    if (key >= '0' && key <= '9') {
      Serial.print("*"); // Imprimir la tecla presionada (se cambió println por print)
      userInput = userInput * 10 + (key - '0'); // Construir el número introducido por el usuario
      contador = contador + 1;
    } 
    else {
      Serial.println("\nLas letras y los símbolos no son válidos"); // Imprimir mensaje de tecla no válida
    }
  }
  
  if (contador == 8) {
    if (userInput == storedNumber) {
      Serial.println("\n¡Contraseña correcta!"); // El número introducido es correcto
    } 
    else {
      Serial.println("\nContraseña incorrecta, intentalo otra vez"); // El número introducido es incorrecto
    }
    userInput = 0; // Restablecer el valor de userInput para la próxima entrada
    contador = 0; // Reiniciar el contador
  }
}
