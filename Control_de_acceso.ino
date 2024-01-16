#include <Keypad.h>

#define ROW_NUM     4 // four rows
#define COLUMN_NUM  4 // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM]      = {19, 18, 5, 17}; // GPIO19, GPIO18, GPIO5, GPIO17 connect to the row pins
byte pin_column[COLUMN_NUM] = {16, 4, 0, 2};   // GPIO16, GPIO4, GPIO0, GPIO2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

int storedNumber = 1234; // Número previamente almacenado para comparación

void setup() {
  Serial.begin(9600); // Iniciar la comunicación con el puerto serie
}

int userInput = 0; // Variable para almacenar el número introducido por el usuario

void loop() {
  char key = keypad.getKey(); // Obtener la tecla presionada por el usuario
  if (key) {
//    Serial.print("Tecla presada: ");
    if (key >= '0' && key <= '9'){
      Serial.println("*");
    } 
    else{
      Serial.println(" "); }
    
    if (key >= '0' && key <= '9') { // Verificar si la tecla presionada es un dígito
      userInput = userInput * 10 + (key - '0'); // Construir el número introducido por el usuario
    } 
    else if (key == 'A') { // Si se presiona la tecla 'A'
      if (userInput == storedNumber) {
        Serial.println("¡Número correcto!"); // El número introducido es correcto
      } 
      else {
        Serial.println("Número incorrecto, vuelve a intentar"); // El número introducido es incorrecto
        userInput = 0; // Restablecer el valor de userInput para la próxima entrada
      }
      
    }
  }
}
