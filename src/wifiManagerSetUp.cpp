#include "WiFiManagerSetup.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include "LcdSetup.h"
bool wifiConnected = false;
WiFiManager wifiManager;

void setUpWifi(SemaphoreHandle_t wifiSemaphore,SemaphoreHandle_t lcdSemaphore) {
  xSemaphoreTake(wifiSemaphore,portMAX_DELAY);
  xSemaphoreTake(lcdSemaphore,portMAX_DELAY);
  if (!wifiManager.autoConnect("ESP32-Config")) {
    Serial.println("Fallo al conectar y tiempo de espera alcanzado");
    displayInfoOnLCD("Wifi: Desconectado", "");
    delay(5000);
    ESP.restart(); 
  }
  Serial.print("Potencia de transmisión WiFi: ");
  Serial.println(WiFi.RSSI());
  Serial.println("Conectado a WiFi!");
  displayInfoOnLCD("wifi:conectado", WiFi.RSSI() == 0 ? "RSSI: 0" : String(WiFi.RSSI()).c_str());
  wifiConnected = true;
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xSemaphoreGive(wifiSemaphore);
}

void reconectWiFi(SemaphoreHandle_t lcdSemaphore){
  if (WiFi.status() != WL_CONNECTED) {
    xSemaphoreTake(lcdSemaphore,portMAX_DELAY);
    displayInfoOnLCD("disconected WiFi","reconnecting...");
    Serial.println("Conexión WiFi perdida, reconectando...");
    WiFi.reconnect();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xSemaphoreGive(lcdSemaphore);
  }
}