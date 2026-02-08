#ifndef PUBLISHING_INIT_H
#define PUBLISHING_INIT_H

#include <Preferences.h>
#include <Arduino.h>

// Variables globales accesibles
extern Preferences nvs;
extern uint32_t lastSequence;
extern String sessionId;
extern uint32_t lastConfirmedSequence;


// Función de inicialización
void initPublishing();

#endif
