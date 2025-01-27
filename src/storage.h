#ifndef SPIFFSMANAGER_H
#define SPIFFSMANAGER_H
#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 



void setupSPIFFS(); 
void saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20, int toSend);
//void sendStoredData();   
void readAndUpdateCSV();
#endif 