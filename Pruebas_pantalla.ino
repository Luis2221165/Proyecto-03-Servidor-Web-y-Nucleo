#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include <string.h> // Para usar memcpy

#define TFT_CS     PB0
#define TFT_RST    PC1
#define TFT_DC     PA4

#define SEG_A PB1
#define SEG_B PB2
#define SEG_C PC6
#define SEG_D PB8
#define SEG_E PB9
#define SEG_F PC8
#define SEG_G PB12

MCUFRIEND_kbv tft; // Crear el objeto para la pantalla TFT
HardwareSerial Serial5(UART5); // Inicializa UART5 como Serial5

int lastValue = -1; 
int rectX = 150;    
int rectY = 50;     
int tireSize = 14;  
int spacingX = 60;  
int spacingY = 90;  

bool rectStates[8] = {false, false, false, false, false, false, false, false}; 

void setup() {
  Serial.begin(9600);  
  Serial5.begin(9600); // Configura la velocidad de UART5
  
  uint16_t ID = tft.readID();  
  tft.begin(ID);
  tft.setRotation(3);  
  
  tft.fillScreen(0xFFFF);
  
  tft.setTextColor(0x0000);
  tft.setTextSize(2);
  tft.setCursor(110, 10);
  tft.print("Parqueo-Matic");
  
  tft.fillRect(10, 10, 50, 50, 0x0400); 
  tft.setTextColor(0xFFFF); 
  tft.setTextSize(2); 
  tft.setCursor(18, 25);
  tft.print("UVG");

  tft.setTextColor(0xF800); 
  tft.setTextSize(6);       
  tft.setCursor(20, 100);    

  drawVerticalLines();
  drawHorizontalLine();
  displayInactiveRectangles();

  // Configurar pines del display de 7 segmentos
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
}

void drawVerticalLines() {
  uint16_t lineColor = 0xFFE0;
  int startX = 68;  
  int lineWidth = 6; 
  int lineHeight = 160; 

  for (int i = 0; i < 5; i++) {
    tft.fillRect(startX + (i * spacingX), 60, lineWidth, lineHeight, lineColor);
  }
}

void drawHorizontalLine() {
  uint16_t lineColor = 0xFFFF;
  int startY = 122;
  int lineWidth = 22;
  int lineLength = 270;

  tft.fillRect(50, startY, lineLength, lineWidth, lineColor);
}

void drawRectangleWithTires(int x, int y, int width, int height, uint16_t color, bool isSecondRow) {
  tft.fillRect(x, y, width, height, color);
  tft.fillRect(x, y, tireSize, tireSize, 0x0000);                     
  tft.fillRect(x + width - tireSize, y, tireSize, tireSize, 0x0000);  
  tft.fillRect(x, y + height - tireSize, tireSize, tireSize, 0x0000); 
  tft.fillRect(x + width - tireSize, y + height - tireSize, tireSize, tireSize, 0x0000);

  int smallRectWidth = 30;
  int smallRectHeight = 10;
  int smallRectX = x + (width - smallRectWidth) / 2;

  int smallRectY = isSecondRow ? y + height + 2 : y - smallRectHeight - 2;

  tft.fillRect(smallRectX, smallRectY, smallRectWidth, smallRectHeight, 0x5AF7);

  int circleRadius = 5;
  int circleX = smallRectX + smallRectWidth / 2;
  int circleY = smallRectY + smallRectHeight / 2;

  tft.fillCircle(circleX, circleY, circleRadius, 0x07E0);
}

void clearRectangle(int index) {
  int x = 80 + (index % 4) * spacingX;
  int y = 60 + (index / 4) * spacingY;
  tft.fillRect(x, y, 40, 60, 0xFFFF); 
  drawVerticalLines();
}

void displayInactiveRectangles() {
  int inactiveCount = 0;
  for (int i = 0; i < 8; i++) {
    if (!rectStates[i]) {
      inactiveCount++;
    }
  }

  tft.fillRect(20, 100, 30, 50, 0xFFFF);
  tft.setCursor(20, 100);
  tft.print(inactiveCount);

  // Mostrar el número de espacios libres en el display de 7 segmentos
  displayNumber(inactiveCount);
}

void displayNumber(int number) {
  bool segments[7];
  
  // Matriz de segmentos para cada número (0 a 9)
  const bool numbers[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
  };

  memcpy(segments, numbers[number], 7 * sizeof(bool));

  digitalWrite(SEG_A, segments[0]);
  digitalWrite(SEG_B, segments[1]);
  digitalWrite(SEG_C, segments[2]);
  digitalWrite(SEG_D, segments[3]);
  digitalWrite(SEG_E, segments[4]);
  digitalWrite(SEG_F, segments[5]);
  digitalWrite(SEG_G, segments[6]);
}

void loop() {
  if (Serial5.available()) {
    String receivedData = Serial5.readString(); 
    int value = receivedData.toInt();

    if (value >= 1 && value <= 8) {
      if (!rectStates[value - 1]) {
        rectStates[value - 1] = true;
        int x = 80 + ((value - 1) % 4) * spacingX;
        int y = 60 + ((value - 1) / 4) * spacingY;

        uint16_t color;
        switch (value) {
          case 1: color = 0x07E0; break; 
          case 2: color = 0xF800; break; 
          case 3: color = 0x001F; break; 
          case 4: color = 0x07E0; break; 
          case 5: color = 0xF800; break; 
          case 6: color = 0x001F; break; 
          case 7: color = 0x780F; break; 
          case 8: color = 0x001F; break; 
        }

        drawRectangleWithTires(x, y, 40, 60, color, (value > 4));
        displayInactiveRectangles();
      }
    } 
    else if (value <= -1 && value >= -8) {
      int rectIndex = abs(value) - 1;
      rectStates[rectIndex] = false;
      clearRectangle(rectIndex);
      displayInactiveRectangles();
    }
  }
}
