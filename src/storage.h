#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H
#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
#include <Arduino.h>      
extern SemaphoreHandle_t spiffsMutex;
extern SemaphoreHandle_t sequenceMutex;


void setupSPIFFS(); 
bool saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20, int sequence);
bool sendStoredData();
void recoverSPIFFSState();    
void readAndUpdateCSV();
#endif 