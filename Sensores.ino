#include <HardwareSerial.h>

// Pines de los sensores IR
const int sensorIR1 = PA10;
const int sensorIR2 = PA11;
const int sensorIR3 = PA12;
const int sensorIR4 = PA13;
const int sensorIR5 = PA14;
const int sensorIR6 = PA15;
const int sensorIR7 = PB0;
const int sensorIR8 = PB1;

// Pines de los LEDs bicolor (ánodo común)
const int ledR1 = PB2; // Rojo del LED 1
const int ledG1 = PB3; // Verde del LED 1
const int ledR2 = PB4; // Rojo del LED 2
const int ledG2 = PB5; // Verde del LED 2
const int ledR3 = PB6; // Rojo del LED 3
const int ledG3 = PB7; // Verde del LED 3
const int ledR4 = PB8; // Rojo del LED 4
const int ledG4 = PB9; // Verde del LED 4
const int ledR5 = PB10; // Rojo del LED 5
const int ledG5 = PC11; // Verde del LED 5
const int ledR6 = PB12; // Rojo del LED 6
const int ledG6 = PB13; // Verde del LED 6
const int ledR7 = PB14; // Rojo del LED 7
const int ledG7 = PB15; // Verde del LED 7
const int ledR8 = PC0;  // Rojo del LED 8
const int ledG8 = PC1;  // Verde del LED 8

// Variables para almacenar el último estado de los sensores
int lastStateIR1 = HIGH;
int lastStateIR2 = HIGH;
int lastStateIR3 = HIGH;
int lastStateIR4 = HIGH;
int lastStateIR5 = HIGH;
int lastStateIR6 = HIGH;
int lastStateIR7 = HIGH;
int lastStateIR8 = HIGH;

HardwareSerial Serial5(UART5); // Inicializa UART5 como Serial5
HardwareSerial Serial4(UART4); // Inicializa UART4 como Serial4

void setup() {
    Serial5.begin(9600); // Inicia la comunicación en UART5
    Serial4.begin(9600); // Inicia la comunicación en UART4
    Serial.begin(9600);  // Para monitorear en la consola serie del Arduino IDE

    // Configuración de pines de sensores IR
    pinMode(sensorIR1, INPUT);
    pinMode(sensorIR2, INPUT);
    pinMode(sensorIR3, INPUT);
    pinMode(sensorIR4, INPUT);
    pinMode(sensorIR5, INPUT);
    pinMode(sensorIR6, INPUT);
    pinMode(sensorIR7, INPUT);
    pinMode(sensorIR8, INPUT);

    // Configuración de pines de LEDs bicolor
    pinMode(ledR1, OUTPUT);
    pinMode(ledG1, OUTPUT);
    pinMode(ledR2, OUTPUT);
    pinMode(ledG2, OUTPUT);
    pinMode(ledR3, OUTPUT);
    pinMode(ledG3, OUTPUT);
    pinMode(ledR4, OUTPUT);
    pinMode(ledG4, OUTPUT);
    pinMode(ledR5, OUTPUT);
    pinMode(ledG5, OUTPUT);
    pinMode(ledR6, OUTPUT);
    pinMode(ledG6, OUTPUT);
    pinMode(ledR7, OUTPUT);
    pinMode(ledG7, OUTPUT);
    pinMode(ledR8, OUTPUT);
    pinMode(ledG8, OUTPUT);
}

void loop() {
    checkSensor(sensorIR1, lastStateIR1, 1, ledR1, ledG1);
    checkSensor(sensorIR2, lastStateIR2, 2, ledR2, ledG2);
    checkSensor(sensorIR3, lastStateIR3, 3, ledR3, ledG3);
    checkSensor(sensorIR4, lastStateIR4, 4, ledR4, ledG4);
    checkSensor(sensorIR5, lastStateIR5, 5, ledR5, ledG5);
    checkSensor(sensorIR6, lastStateIR6, 6, ledR6, ledG6);
    checkSensor(sensorIR7, lastStateIR7, 7, ledR7, ledG7);
    checkSensor(sensorIR8, lastStateIR8, 8, ledR8, ledG8);

    delay(100);
}

void checkSensor(int pin, int &lastState, int sensorID, int ledR, int ledG) {
    int currentState = digitalRead(pin);
    if (currentState != lastState) {
        if (currentState == LOW) {
            Serial5.println(sensorID);
            Serial4.println(sensorID);  // Envía el mismo dato por UART4
            Serial.print("Sensor ");
            Serial.print(sensorID);
            Serial.println(" detecta objeto");

            // Cambia LED a rojo
            digitalWrite(ledR, HIGH);
            digitalWrite(ledG, LOW);
        } else {
            Serial5.println(-sensorID);
            Serial4.println(-sensorID); // Envía el mismo dato por UART4
            Serial.print("Sensor ");
            Serial.print(sensorID);
            Serial.println(" no detecta objeto");

            // Cambia LED a verde
            digitalWrite(ledR, LOW);
            digitalWrite(ledG, HIGH);
        }
        lastState = currentState;
    }
}
