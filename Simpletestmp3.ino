#include <SoftwareSerial.h>

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

int currentSongIndex = 1; // Índice de la canción actual

void setup()
{
  mySerial.begin(9600);
  delay(500);
  sendCommand(CMD_SEL_DEV, DEV_TF);
  delay(200);
  sendCommand(CMD_PLAY_W_INDEX, currentSongIndex); // Reproducir la primera canción
}

void loop()
{
  // Ejemplo de cómo cambiar a la siguiente canción después de un retraso de 5 segundos (5000 milisegundos)
  delay(13000);
  currentSongIndex++; // Incrementar el índice de la canción
  sendCommand(CMD_PLAY_W_INDEX, currentSongIndex); // Enviar el comando para reproducir la próxima canción
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
