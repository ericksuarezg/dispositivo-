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
  if (!wifiManager.autoConnect("ESP32-Config")) {
    Serial.println("Fallo al conectar y tiempo de espera alcanzado");
    displayInfoOnLCD("Wifi: Desconectado", "");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    displayInfoOnLCD("Desplegando red", "  ESP32-Config");
    delay(5000);
    ESP.restart(); 
  }
  Serial.print("Potencia de transmisión WiFi: ");
  Serial.println(WiFi.RSSI());
  Serial.println("Conectado a WiFi!");
  String ssid = WiFi.SSID(); // Obtiene el nombre de la red
  displayInfoOnLCD(ssid.c_str(), ("RSSI: " + String(WiFi.RSSI())).c_str());
  vTaskDelay(2000 / portTICK_PERIOD_MS);
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