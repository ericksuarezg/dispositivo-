#ifndef TIMERCONTROL_H
#define TIMERCONTROL_H

#include <freertos/FreeRTOS.h>  // Este debe ser el primer encabezado relacionado con FreeRTOS
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <Ticker.h>

// Definir los temporizadores globales
extern Ticker timerTask1;
extern Ticker timerTask2;
extern Ticker timerTask3;


// Declarar las tareas programadas
void tareaProgramada1(SemaphoreHandle_t lcdSemaphore, float& temperatureCDs18b20, float &temperaturaDHT, float &humedadDHT, String payload, String dataTime, float tempDHT, float humedad, float tempDS18B20, int toSend, float humedadRelativa, float temperaturaDS18);
void tareaProgramada2();

// Declarar la función del sensor DS18B20
void ds18b20ReadTemperature(SemaphoreHandle_t lcdSemaphore, float& temperatureCDs18b20);
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad);
void saveDataToCSV(String payload, String dataTime, float tempDHT, float humedad, float tempDS18B20, int toSend);
void sendStoredData(SemaphoreHandle_t lcdSemaphore);
void publishData(SemaphoreHandle_t lcdSemaphore, float temperaturaDHT,float humedadRelativa, float temperaturaDS18);


// Función para iniciar los temporizadores
void startTimers(time_t adjustedTime);

#endif