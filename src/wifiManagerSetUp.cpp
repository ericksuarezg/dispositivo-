#include "WiFiManagerSetup.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include "LcdSetup.h"
bool wifiConnected = false;
WiFiManager wifiManager;

void setUpWifi(SemaphoreHandle_t wifiSemaphore,SemaphoreHandle_t lcdSemaphore) {
  xSemaphoreTake(wifiSemaphore,portMAX_DELAY);
  xSemaphoreTake(lcdSemaphore,portMAX_DELAY);
  displayInfoOnLCD("conectando WiFi.",".....");
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  wifiManager.setTimeout(60);
  if (wifiManager.autoConnect("ESP32-Config")) {
    Serial.print("Potencia de transmisión WiFi: ");
    Serial.println(WiFi.RSSI());
    Serial.println("Conectado a WiFi!");
    String ssid = WiFi.SSID(); // Obtiene el nombre de la red
    displayInfoOnLCD(ssid.c_str(), ("RSSI: " + String(WiFi.RSSI())).c_str());
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    displayInfoOnLCD("wifi:conectado", WiFi.RSSI() == 0 ? "RSSI: 0" : String(WiFi.RSSI()).c_str());
    wifiConnected = true;
  }else{
    Serial.println("Tiempo de espera agotado para conectar a WiFi");
    displayInfoOnLCD("Tiempo agotado", "No conectado");
  }
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xSemaphoreGive(wifiSemaphore);
}


void reconectWiFi(SemaphoreHandle_t lcdSemaphore) {
  if (!wifiConnected) {
    Serial.println("Intentando conectar WiFi desde reconectWiFi...");
    if (xSemaphoreTake(lcdSemaphore, 5000 / portTICK_PERIOD_MS) == pdTRUE) {
      displayInfoOnLCD("Intentando reconectar", "al WiFi...");
      wifiManager.setTimeout(30); 
      wifiConnected = wifiManager.autoConnect("ESP32-Config");
      if (wifiConnected) {
        Serial.println("Reconexión exitosa desde reconectWiFi.");
        displayInfoOnLCD("Reconectado", "WiFi exitoso");
      } else {
        Serial.println("Falló la reconexión desde reconectWiFi.");
        displayInfoOnLCD("Reconexión fallida", "Intentando luego...");
      }
      xSemaphoreGive(lcdSemaphore);
    }
  } else if (WiFi.status() != WL_CONNECTED) {
    if (xSemaphoreTake(lcdSemaphore, 5000 / portTICK_PERIOD_MS) == pdTRUE) {
      displayInfoOnLCD("WiFi perdido", "Reconectando...");
      Serial.println("Conexión WiFi perdida, reconectando...");
      WiFi.reconnect();
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
    }
  }
}


bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}