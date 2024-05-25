#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <WebSocketsServer.h>


SoftwareSerial mySerial(14, 12); // RX, TX
DFRobotDFPlayerMini mp3;


// Configura tus credenciales WiFi
const char* ssid = "Internet";
const char* password = "qwertyuiop";



WebSocketsServer webSocket(81); // Crea un servidor WebSocket en el puerto 81

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d URL: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // Puedes enviar un mensaje al cliente
        webSocket.sendTXT(num, "Hello Client!");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      // Echo message back to client
      webSocket.sendTXT(num, payload);
      handleSoundCommand((char *)payload);
      break;
  }
}

void handleSoundCommand(char* payload) {
  int soundNumber = 0;  // Variable para almacenar el número de sonido a reproducir

  // Compara el mensaje recibido con comandos conocidos
  if (strcmp(payload, "Despertar") == 0) {
    soundNumber = 1;
  } else if (strcmp(payload, "Desayunar") == 0) {
    soundNumber = 2;
  } else if (strcmp(payload, "Almorzar") == 0) {
    soundNumber = 3;
  } else if (strcmp(payload, "Cenar") == 0) {
    soundNumber = 4;
  } else if (strcmp(payload, "Merienda") == 0) {
    soundNumber = 5;
  } else if (strcmp(payload, "Taller") == 0) {
    soundNumber = 6;
  } else if (strcmp(payload, "Clase") == 0) {
    soundNumber = 7;
  }

  // Si se encontró un número de sonido válido, reproducir el sonido
  if (soundNumber > 0) {
    playSound(soundNumber);
  } else {
    Serial.println("Comando no reconocido.");
  }
}

void playSound(int soundNumber) {
  Serial.print("Reproduciendo sonido ");
  Serial.println(soundNumber);
  mp3.play(soundNumber); 
  delay(1000); // Espera 1 segundo antes de la próxima acción
}


void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);

  if (!mp3.begin(mySerial)) {  
    Serial.println("Error al iniciar el reproductor DFPlayer Mini. ras");
    while (true);
  }

  WiFi.begin(ssid, password);

  // Espera hasta que la conexión se establezca
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Inicia el servidor WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}
