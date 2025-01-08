#ifndef WiFiManagerSetup_h
#define WiFiManagerSetup_h
#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
void setUpWifi(SemaphoreHandle_t wifiSemaphore,SemaphoreHandle_t lcdSemaphore);
void reconectWiFi(SemaphoreHandle_t lcdSemaphore);
bool isWiFiConnected();

#endif