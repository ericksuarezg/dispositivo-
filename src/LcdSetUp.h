#ifndef LcdSetup_h
#define LcdSetup_h
#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 



void setUpLcd(SemaphoreHandle_t wifiSemaphore);
void displayInfoOnLCD(const char* line1, const char* line2);
void displayDataOnLCDofDHT(float temperature, float humidity);
void displayDataOnLCDofDbs18b20(float temperatureCDs18b20);
void displayDataOnLCD(float temperature, float humidity,float temperatureCDs18b20);

#endif