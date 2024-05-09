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
const char* mqttServer = "10.0.110.111";
const int mqttPort = 1883;
const char* mqttUser = "ADMIN"; //sensor o control
const char* mqttPassword = "ASIX";

//********* Nom del client MQTT
const char* clientID = "Patata";

//********* PANTALLA OLED
#define oled 32


Adafruit_SSD1306 display = Adafruit_SSD1306(128, oled, &Wire);
static const unsigned char PROGMEM imgNuclear[512] = { 
0x40, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x60, 0x00, 0x00, 0x18,
   0x00, 0x00, 0x00, 0x02, 0x60, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x06,
   0x30, 0x01, 0x00, 0x32, 0x00, 0x00, 0x00, 0x06, 0x30, 0x01, 0x00, 0x26,
   0x00, 0x00, 0x00, 0x0c, 0x90, 0x01, 0x00, 0x26, 0x00, 0x00, 0x00, 0x0c,
   0xd0, 0x01, 0x00, 0x2e, 0x00, 0x04, 0x00, 0x08, 0xd0, 0x01, 0x00, 0x2e,
   0x00, 0x04, 0x00, 0x0f, 0x50, 0x01, 0x00, 0x2a, 0x00, 0x0c, 0x00, 0x0e,
   0x50, 0x01, 0x00, 0x2a, 0x00, 0x1c, 0x00, 0x0e, 0x50, 0x01, 0x00, 0x2a,
   0x00, 0x19, 0x00, 0x0c, 0x50, 0x01, 0x00, 0x2a, 0x00, 0x13, 0x00, 0x0c,
   0x50, 0x01, 0x00, 0x2a, 0x00, 0x13, 0x00, 0x00, 0x54, 0x09, 0x02, 0xaa,
   0x11, 0x17, 0x11, 0x00, 0x56, 0x0d, 0x02, 0xaa, 0x19, 0x97, 0x33, 0x03,
   0x56, 0x1d, 0x02, 0xaa, 0x33, 0x95, 0x77, 0x03, 0x53, 0x1d, 0x2a, 0xaa,
   0x33, 0xd5, 0x66, 0x03, 0x53, 0x3d, 0x6a, 0xaa, 0x67, 0xd5, 0x4c, 0x0b,
   0x51, 0x3d, 0x6a, 0xaa, 0x6e, 0x55, 0x4c, 0x0b, 0x41, 0x2d, 0xea, 0xa8,
   0x4c, 0x54, 0x58, 0x0b, 0x4d, 0x2d, 0xea, 0xa8, 0x58, 0x54, 0x59, 0x0b,
   0x4d, 0x3d, 0xaa, 0xa9, 0x58, 0x54, 0x53, 0x0b, 0x5d, 0x2d, 0xaa, 0xa3,
   0x58, 0x54, 0x47, 0x0b, 0x5d, 0x2d, 0x2a, 0xa3, 0x5a, 0x54, 0x47, 0x0b,
   0x75, 0x2d, 0x2a, 0xa6, 0x5a, 0x54, 0x4d, 0x0b, 0x75, 0x2d, 0x2a, 0xae,
   0x5a, 0x50, 0x5d, 0x0b, 0x65, 0x2d, 0xaa, 0xac, 0x5a, 0x51, 0x59, 0x0b,
   0x45, 0x2d, 0xaa, 0xa9, 0x5a, 0x51, 0x51, 0x0b, 0x45, 0x6d, 0xaa, 0xa9,
   0x5a, 0x53, 0x51, 0x0b, 0x05, 0x6d, 0xaa, 0xab, 0x5e, 0x57, 0x51, 0x0b,
   0x35, 0xcd, 0xaa, 0xab, 0x5e, 0x56, 0x55, 0x0b, 0x35, 0xcd, 0xaa, 0xaa,
   0x5c, 0x54, 0x55, 0x08, 0x65, 0x0d, 0xaa, 0xa8, 0x5c, 0x54, 0x55, 0x08,
   0x65, 0x2d, 0xaa, 0xa8, 0x59, 0x54, 0x55, 0x08, 0x4d, 0x2d, 0xaa, 0xa8,
   0x59, 0x54, 0x55, 0x08, 0x5d, 0x6d, 0xa8, 0x20, 0x43, 0x54, 0x55, 0x08,
   0x59, 0xed, 0xa8, 0x20, 0x63, 0x54, 0x55, 0x0a, 0x73, 0xcd, 0xac, 0x20,
   0x66, 0xd4, 0x5d, 0x0e, 0x73, 0x98, 0xaf, 0x30, 0x3c, 0xd4, 0x1d, 0x0e,
   0x66, 0x98, 0x87, 0x38, 0x3c, 0x94, 0x19, 0x0e, 0x46, 0x30, 0x83, 0x18,
   0x18, 0x94, 0x11, 0x0c, 0x44, 0x10, 0x82, 0x08, 0x18, 0x14, 0x11, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00 };

byte pin_rows[ROW_NUM]      = {19, 18, 5, 27}; // GPIO19, GPIO18, GPIO5, GPIO27 conectados a los pines de las filas
byte pin_column[COLUMN_NUM] = {26, 4, 0, 2};   // GPIO26, GPIO4, GPIO0, GPIO2 conectados a los pines de las columnas

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

WiFiClient espClient;
PubSubClient client(espClient);

//********* Certificats
const char* CA_cert = "-----BEGIN CERTIFICATE-----\n" \
"MIID9TCCAt2gAwIBAgIUcjhFez/0M99teNDLn/vdzOW7/1cwDQYJKoZIhvcNAQEL\n" \
"BQAwgYkxCzAJBgNVBAYTAmVzMRIwEAYDVQQIDAl0YXJyYWdvbmExETAPBgNVBAcM\n" \
"CGNhbGFmZWxsMQwwCgYDVQQKDANpZGsxDDAKBgNVBAsMA2lkazEMMAoGA1UEAwwD\n" \
"aWRrMSkwJwYJKoZIhvcNAQkBFhpyc2FuY2hlejNAaW5zY2FtaWRlbWFyLmNhdDAe\n" \
"Fw0yMzEyMTMxNjMyNTZaFw0yODEyMTIxNjMyNTZaMIGJMQswCQYDVQQGEwJlczES\n" \
"MBAGA1UECAwJdGFycmFnb25hMREwDwYDVQQHD4AhjYWxhZmVsbDEMMAoGA1UECgwD\n" \
"aWRrMQwwCgYDVQQLDANpZGsxDDAKBgNVBAMMA2lkazEpMCcGCSqGSIb3DQEJARYa\n" \
"cnNhbmNoZXozQGluc2NhbWlkZW1hci5jYXQwggEiMA0GCSqGSIb3DQEBAQUAA4IB\n" \
"DwAwggEKAoIBAQChT6Rw7lsy54+h9yxWChfOiW4d/nSv1QUTk4cDU7E81X46ApEV\n" \
"zpvniUwL67UynYklgTfX4Ga9/WdE7dRAnlP65v2zD1hEj6iNJMbgugy66Xq9r3md\n" \
"io/wRdI6hFaVaAcOu9lf7njW6Z004iYaMfiRhLAwqkQAwSDJG6HERBjHu0RhwYzY\n" \
"Upk1zr+sLVskN0FAOVgqGB4ruIv/WLRJQIOzMgAsw/ONQ2Lqgu/e6LoXqOCDNXA8\n" \
"fE0jcTwcl8IMZiz9GafGUlDO34UyQXf9YhB0Ojk5P2zArtzBH2YNHUBOe0hj92zR\n" \
"cXOPplMTDdvNLh7ISWh+vCf5BHn6eUOWXSlHAgMBAAGjUzBRMB0GA1UdDgQWBBS1\n" \
"j07j4Nvf2HhFP6KB1zjVpMGwMzAfBgNVHSMEGDAWgBS1j07j4Nvf2HhFP6KB1zjV\n" \
"pMGwMzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQBngF8Mg2TK\n" \
"YCF5jamItsColaNoY+WTut7j2bmmt/Xb5rY7mAseziRypPKww69qmGkAh7VphgqC\n" \
"xFpwU/LsR8dvGqtmeOQ4eHtFOlBemEsgGDKpHIR1FI2JDg44lofEWxvzYyteMAJ8\n" \
"7hxTG/t3tuhUO+Z6WjFmkrd/Z9NGc+YYfXjpeAcxjJTwey/u2aDMLTTo5PEO8L0d\n" \
"7Oc5L+FzqB+0eJOgp3jfFsOFEu0e9nNnB/NO4VVX9E4gvQKFKzc8eYPi1zaXesj/\n" \
"cmy5t3ZR8bvAEIQOCQqWEIXIn1oLWACNjUvgUil2WH15WV4L35uq2aj6xVRP1GAv\n" \
"iBLz9x3R4EGj\n" \
"-----END CERTIFICATE-----\n";
const char* ESP_RSA_key = "-----BEGIN ENCRYPTED PRIVATE KEY-----\n" \ 
"MIIFHDBOBgkqhkiG9w0BBQ0wQTApBgkqhkiG9w0BBQwwHAQIY2LC8sOoCyICAggA\n" \ 
"MAwGCCqGSIb3DQIJBQAwFAYIKoZIhvcNAwcECCPqUeT+q1QCBIIEyO7dl0c3RYYz\n" \ 
"w8aPUUNDv+5z3ed56OfNw+QPXr6k7l3NhYB/NinxTdQ+HmLkyTIUU3FaC1D05pQ6\n" \ 
"iTv+lw7w3HFb9Q4DGbWaYa4ode7E1/GclXEkh81MR9v0uzFKQNOxMr4CnKZiKVat\n" \ 
"0tp7TD+UXi7zYMawvnHdr+0q0rLJhJ/doFqiEXW2vZolAvQ0diBh1bX90dqiyP8z\n" \ 
"+DPHZJVTIHwqSQAUVab37KDO1qZVxXtQAsQ/5Jb3+8wZkI1eORpTeUtNZSzun2sw\n" \ 
"RINARU1xY3SEUiwRYcKb3lvsDjEOX2zb5iMkFUzidSAR81TwHl6QwfEfbTYWpGC4\n" \ 
"OsbCWw2l89Z6Ktsp6vj0kEXfbmCsMVtUeiPBt172zYKiXDPJjNJBLGsI3xxuaa/z\n" \ 
"25v153uiPxX4HgJGauVRxx/u4bOLFtgjrNHnEIu4eso+uzmts2bjZssvXd/145sB\n" \ 
"vzGfLyuQ/XP0tO8eEjrZdIp8oIJYOuwTxbWgYjhQxkqvA9SoN0FmGgseuKJH71/R\n" \ 
"12L7+LCFm3XO32t9wM9sDVESZzKDWna3CPo/vGUQRbHMi40MGB11M4WBUlT84oqc\n" \ 
"hL3D+BMNFq2r3R+aqgynCoXKWvYK24UZ4/ivDEH7Kkrgcc8gp3Nwwp8mAc8d1BEc\n" \ 
"T5sRYE/8wapCULOpGlbVE7AxQ8n/bb6N/30AlL3oe3bLe+ghwdt5O+2EAVAuEKZF\n" \ 
"OYqZmAnTYMEOIw+e9TRqO2tXZB9yO6hYs7GV4FbnXtoQR6hnhARqSYBsTvl+jtC0\n" \ 
"5eUoR8IVJ7G3CAHrigh5NEyZSlZsf9HnpWjnJpQ/urqyhOem4/GSUmsVZei+v6vg\n" \ 
"RcSfkYDJZHtJ7TJt7myEl8Y67IrCunpaFRtEbY6JbFD/tHIjDLs4BrbDCI0toe7l\n" \ 
"bDsf6Lx8cQBDI4WJurByjvoM8M8kNwh2dE7aUk+zWghyJWa7KAO58uik49MW51r5\n" \ 
"XQTFJch0rlLBMJIr0mM+ozyiW6HIkyu2Y7sxLLoq5Yl7QpKo60JuOT1cisLrrry4\n" \ 
"a+beiGSeWi0mpGuHXQIMzJUEAyMWiPPrRDXEn6iGkUPVDSKNVxLxnVlb3zqPPhtl\n" \ 
"TpT1rrh13Nw7Ye1YpIMpup1DO3FrPWa4xaHg55MjY2KBoBxSN6I3U0c1QnK0HosG\n" \ 
"xyz/rzznETse9Uw2CuPSXfSCvtic72u4hZlV7LWLRUMjOVL2JlNqznVn1GsP4ZCX\n" \ 
"golBzy/IxNngTEOxz6AhYMc9ajbNwgDq+c6NYD/AaVkrhAK6ImArfAEcSai34sEX\n" \ 
"xhtxVUvq8+MBMMgkKVFdC3TtpfO4mlIxYkmStah34LschJRds4+mC/OwWau7XIb2\n" \ 
"OcwvPVBmcxPzxRR8ODO39tU4DLpgXiippiVrPteEslUOvTBFMAJSnIO0pRVuNL2E\n" \ 
"3/A9OexKRn2v+YWFjFObmW4HcVAzsgfS1qxMVctGj36M3rLwtp2Pj+XC0qd7T9b9\n" \ 
"gEG17SHKSwFE2L64dvV5PBJnnb1yIoqPkGwOWO1JXkrCHr+6/UlQeXMYUBAXBPJJ\n" \ 
"QTlXS3jm9s8u8/jm0oAltGl7AVsMWl/XHCXABcMdzOc9Yl7fgZ6mFg7T7RjurBLi\n" \ 
"f87Hqnhvf2pGJs347MUAaQ==\n" \ 
"-----END ENCRYPTED PRIVATE KEY-----\n";
const char* ESP_CA_cert = "-----BEGIN CERTIFICATE-----\n" \
"MIIDmzCCAoMCFF68Fu9EWt+7Vn821svAoGrFwLMYMA0GCSqGSIb3DQEBCwUAMIGJ\n" \
"MQswCQYDVQQGEwJlczESMBAGA1UECAwJdGFycmFnb25hMREwDwYDVQQHDAhjYWxh\n" \
"ZmVsbDEMMAoGA1UECgwDaWRrMQwwCgYDVQQLDANpZGsxDDAKBgNVBAMMA2lkazEp\n" \
"MCcGCSqGSIb3DQEJARYacnNhbmNoZXozQGluc2NhbWlkZW1hci5jYXQwHhcNMjMx\n" \
"MjEzMTYzNDIxWhcNMjQxMjA3MTYzNDIxWjCBiTELMAkGA1UEBhMCZXMxEjAQBgNV\n" \
"BAgMCXRhcnJhZ29uYTERMA8GA1UEBwwIY2FsYWZlbGwxDDAKBgNVBAoMA2lkazEM\n" \
"MAoGA1UECwwDaWRrMQwwCgYDVQQDDANpZGsxKTAnBgkqhkiG9w0BCQEWGnJzYW5j\n" \
"aGV6M0BpbnNjYW1pZGVtYXIuY2F0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n" \
"CgKCAQEAwGBN4wWGxyhlWHgsh/9Jy1Bpm9Ust6TF3x4Esnoa5srvhWVjVExxFNtq\n" \
"Zq+xJqy8LcvY+XjAaOsIrLRJip+xFRn82m9a7ire3OagNzwcULnQE/uCFTPKloLI\n" \
"KBE3U4Ne4pLH2KUxEPQvmfB4c5HGV0hpOi596Oafk+ubyB+cmDUpShl1mWDWjZn+\n" \
"+J/nTn/ks3HhQby+KQa6F6cFWzZVEh0stRzil/25JoFlgTB/sPha/f7JgLLSNMUU\n" \
"mzqBY5X/82LypmLLud2NDRpMVXhLbhm1izxUISuekM9KUmWq2umUvk11R3zckdZY\n" \
"YKD8Fy1jr85eeW17gR7o1b0weZcxAwIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQBi\n" \
"SF4yZPvXlP5Sld8akmwoBOkVFhAb/Xbz2SOyPA4HYotJ7SmN1ZqpkX6jd8a/PnzQ\n" \
"jypAIDYvbKX21BzqTHXaRrDwDvex1RORo8UqG+OiAD0Vz+W3nZp5HOJ0KtB0AyxN\n" \
"ZeQJao/LRP2I7ngrQ0jG0UfdUM75AHyLnr/Uo+dTw+opBXYLzK3uQKZdlyVGJow4\n" \
"CQwOKrUjuaGrq8lCXfJwefgzHmQRFWvecLVzdQy77cz97gg7ithXvs7USsgJYyb2\n" \
"lYPCFqDPGoNfZ1+IcXoo6A5vT64456yb+YPAu99ASZe5RIBIPzn4zLq1+KikV8/x\n" \
"2ukIBMbaLaeGtKNe54h6\n" \
"-----END CERTIFICATE-----\n";

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
  display.drawBitmap(0, 0, imgNuclear, 128, 64, SSD1306_WHITE);
  display.display();
  delay(10000);
  display.clearDisplay();
 	display.display();
  display.print("Connectant ...");
  display.display();
  delay(2000);
  display.clearDisplay();
 	display.display();
  display.print("Connectat!");
  display.display();
  delay(1000);
  display.clearDisplay();
 	display.display();
  }

void loop() {
 char key = keypad.getKey();
 display.setCursor(0, 0);
 display.println("Escriu el PIN:\n");
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
     Serial.println("\nS'ha arribat al límit d'errors");
     display.display();
     delay(500);
     display.clearDisplay();
     userInput = 0; // Restablecer el valor de userInput para la próxima entrada
     characterCount = 0;
     // Publicar mensaje MQTT de acceso denegado
     client.publish(MQTT_TOPIC, "¡ALARMA!");
     client.publish(MQTT_TOPIC, "\nS'ha arribat al límit d'errors");
     
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