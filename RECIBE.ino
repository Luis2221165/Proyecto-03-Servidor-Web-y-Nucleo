#include <HardwareSerial.h>

HardwareSerial Serial5(UART5); // Inicializa UART5 como Serial5

void setup() {
  Serial5.begin(9600);  // Configura la velocidad de transmisión en UART5
  Serial.begin(9600);   // Para monitorear en la consola serie de Arduino IDE
}

void loop() {
  if (Serial5.available() > 0) {
    String receivedData = Serial5.readString(); // Lee los datos recibidos
    delay(100); // Pausa para evitar que se acumulen mensajes
  } else {
    delay(100); // Espera en caso de que no haya datos
  }
}
