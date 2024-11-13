#include <Wire.h> // Librería para I2C
#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Reemplaza con tu SSID y contraseña
const char* ssid = "iPhone Ever (2)";
const char* password = "internet12";

WebServer server(80); // Puerto 80 por defecto para HTTP

const int totalSlots = 8; // Número total de espacios de parqueo
bool slotStatus[totalSlots]; // true = ocupado, false = disponible
String slotEntryTime[totalSlots]; // Hora de entrada (ocupado)
String slotExitTime[totalSlots];  // Hora de salida (desocupado)

// Configuración de NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -5 * 3600, 60000); // UTC-5 (ejemplo), actualiza cada 60 segundos

// Dirección I2C del STM32 
const int STM32_ADDRESS = 0x04;

void handleRoot() {
  // Generar la página HTML
  String html = "<!DOCTYPE html><html lang='es'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Estado del Parqueo</title>";
  // Estilos CSS
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; }";
  html += ".container { width: 80%; margin: auto; overflow: hidden; }";
  html += "header { background: #50b3a2; color: #ffffff; padding-top: 30px; min-height: 70px; border-bottom: #e8491d 3px solid; }";
  html += "header h1 { text-align: center; margin: 0; }";
  html += ".status { display: flex; flex-wrap: wrap; justify-content: center; margin-top: 20px; }";
  html += ".slot { background: #ffffff; margin: 10px; padding: 20px; border: 1px solid #dddddd; border-radius: 5px; width: 200px; text-align: center; }";
  html += ".occupied { background-color: #ff4d4d; color: #ffffff; }";
  html += ".available { background-color: #4CAF50; color: #ffffff; }";
  html += ".time-table { width: 100%; margin-top: 20px; }";
  html += "table { width: 100%; border-collapse: collapse; }";
  html += "th, td { padding: 8px 12px; border: 1px solid #dddddd; text-align: center; }";
  html += "th { background-color: #50b3a2; color: #ffffff; }";
  html += "</style>";
  // JavaScript para actualizar los datos automáticamente
  html += "<script>";
  html += "function fetchData() {";
  html += "fetch('/data')";
  html += ".then(response => response.json())";
  html += ".then(data => {";
  html += "for (let i = 1; i <= " + String(totalSlots) + "; i++) {";
  html += "const slotDiv = document.getElementById('slot' + i);";
  html += "if (data['slot' + i]) {";
  html += "slotDiv.className = 'slot occupied';";
  html += "slotDiv.querySelector('p').innerText = 'Ocupado';";
  html += "} else {";
  html += "slotDiv.className = 'slot available';";
  html += "slotDiv.querySelector('p').innerText = 'Disponible';";
  html += "}";
  html += "document.getElementById('entryTime' + i).innerText = data['entryTime' + i];";
  html += "document.getElementById('exitTime' + i).innerText = data['exitTime' + i];";
  html += "}";
  html += "})";
  html += ".catch(error => console.error('Error:', error));";
  html += "}";
  html += "setInterval(fetchData, 1000);"; // Actualizar cada segundo
  html += "window.onload = fetchData;"; // Cargar datos al iniciar
  html += "</script>";
  html += "</head>";
  html += "<body>";
  html += "<header><h1>Estado del Parqueo</h1></header>";
  html += "<div class='container'>";
  html += "<div class='status'>";

  // Generar HTML para cada espacio de parqueo
  for (int i = 0; i < totalSlots; i++) {
    html += "<div class='slot " + String(slotStatus[i] ? "occupied" : "available") + "' id='slot" + String(i + 1) + "'>";
    html += "<h2>Espacio " + String(i + 1) + "</h2>";
    html += "<p>" + String(slotStatus[i] ? "Ocupado" : "Disponible") + "</p>";
    html += "</div>";
  }

  html += "</div>"; // Cerrar .status

  // Tabla de horas
  html += "<div class='time-table'>";
  html += "<table>";
  html += "<tr><th>Espacio</th><th>Hora de Entrada</th><th>Hora de Salida</th></tr>";

  for (int i = 0; i < totalSlots; i++) {
    html += "<tr>";
    html += "<td>Espacio " + String(i + 1) + "</td>";
    html += "<td id='entryTime" + String(i + 1) + "'>" + slotEntryTime[i] + "</td>";
    html += "<td id='exitTime" + String(i + 1) + "'>" + slotExitTime[i] + "</td>";
    html += "</tr>";
  }

  html += "</table>";
  html += "</div>"; // Cerrar .time-table

  html += "</div>"; // Cerrar .container
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleData() {
  // Generar datos en formato JSON
  String json = "{";
  for (int i = 0; i < totalSlots; i++) {
    json += "\"slot" + String(i + 1) + "\":" + (slotStatus[i] ? "true" : "false") + ",";
    json += "\"entryTime" + String(i + 1) + "\":\"" + slotEntryTime[i] + "\",";
    json += "\"exitTime" + String(i + 1) + "\":\"" + slotExitTime[i] + "\"";
    if (i < totalSlots - 1) json += ",";
  }
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200); // Monitor serie

  Wire.begin(); // Iniciar I2C como maestro

  WiFi.begin(ssid, password);
  Serial.println("Conectando a WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado a la red WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Inicializar el estado de los espacios y las horas
  for (int i = 0; i < totalSlots; i++) {
    slotStatus[i] = false; // Asumimos que al inicio todos están disponibles
    slotEntryTime[i] = "N/A";
    slotExitTime[i] = "N/A";
  }

  // Iniciar cliente NTP
  timeClient.begin();

  server.on("/", handleRoot);
  server.on("/data", handleData); // Ruta para obtener datos en formato JSON
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
  receiveDataFromSTM32();
  timeClient.update(); // Actualizar tiempo NTP
}

void receiveDataFromSTM32() {
  Wire.requestFrom(STM32_ADDRESS, 2 * totalSlots); // Solicitar datos del STM32
  
  int i = 0;
  while (Wire.available() && i < totalSlots) {
    int value = Wire.read();
    bool newStatus = (value > 0);
    if (slotStatus[i] != newStatus) {
      slotStatus[i] = newStatus;
      String formattedTime = timeClient.getFormattedTime();
      if (newStatus) {
        slotEntryTime[i] = formattedTime;
        Serial.println("Espacio " + String(i + 1) + " ocupado a las " + formattedTime);
      } else {
        slotExitTime[i] = formattedTime;
        Serial.println("Espacio " + String(i + 1) + " disponible a las " + formattedTime);
      }
    }
    i++;
  }
}


