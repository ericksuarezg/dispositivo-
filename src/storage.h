#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H
#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
#include <Arduino.h>      
extern SemaphoreHandle_t spiffsMutex;


void setupSPIFFS(); 
void saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20, int toSend);
void sendStoredData();
void recoverSPIFFSState();    
void readAndUpdateCSV();
#endif 