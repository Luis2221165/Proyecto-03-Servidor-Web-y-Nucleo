#include <HardwareSerial.h>

HardwareSerial Serial5(UART5); // Inicializa UART5 como Serial5

void setup() {
  Serial5.begin(9600);  // Configura la velocidad de transmisión en UART5
  Serial.begin(9600);   // Para monitorear en la consola serie de Arduino IDE
}

void loop() {
  delay(100); // Espera breve antes de enviar
  delay(1000); // Pausa de 1 segundo entre numeros
}
