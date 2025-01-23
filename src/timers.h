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

float getTemperatureCDs18b20();
float getTemperaturaDHT();
float getHumedad();

// Declarar las tareas programadas
void tareaProgramada1Function(SemaphoreHandle_t lcdSemaphore, float &temperatureCDs18b20, float &temperaturaDHT, float &humedad);
void ds18b20ReadTemperature(SemaphoreHandle_t lcdSemaphore, float &temperatureCDs18b20);
void dhtReading(SemaphoreHandle_t lcdSemaphore, float &temperaturaDHT, float &humedad);


void tareaProgramada2Function();
void tareaProgramada3Function();

// Declarar la función del sensor DS18B20


// Función para iniciar los temporizadores
void startTimers(time_t adjustedTime);

#endif