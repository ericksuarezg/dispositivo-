#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H
#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 



void setupSPIFFS(); 
void saveDataToCSV(float tempDHT, float humedad, float tempDS18B20, unsigned long timestamp);
void sendStoredData(SemaphoreHandle_t lcdSemaphore);
#endif 