#ifndef TIMERCONTROL_H
#define TIMERCONTROL_H

#include <Ticker.h>

// Definir los temporizadores globales
extern Ticker timerTask1;
extern Ticker timerTask2;

// Declarar las tareas programadas
void tareaProgramada1();
void tareaProgramada2();

// Función para iniciar los temporizadores
void startTimers(time_t adjustedTime);

#endif
