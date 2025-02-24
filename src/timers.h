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

float getTemperaturaDHT();
float getHumedad();

// Declarar las tareas programadas
void tareaProgramada1();
void tareaProgramada2();
void tareaProgramada3();


// Función para iniciar los temporizadores
void startTimers(time_t adjustedTime);


#endif