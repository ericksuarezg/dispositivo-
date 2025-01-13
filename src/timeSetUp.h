#ifndef TIME_SETUP_H
#define TIME_SETUP_H

#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include "LcdSetup.h"

// Variables globales
extern time_t baseTime;               // Tiempo base ajustado con Epoch NTP
extern String globalLocalTime;        // Hora local formateada como String
extern unsigned long millisAtSync;    // Valor de millis() al momento de la sincronización
extern long utcOffsetInSeconds;       // Ajuste dinámico de zona horaria

/**
 * Obtiene el desfase horario en segundos basado en la IP del dispositivo.
 * Utiliza el servicio `http://ip-api.com` para determinar la ubicación y zona horaria.
 * @return true si se obtuvo el desfase con éxito, false de lo contrario.
 */
bool fetchUtcOffset();

/**
 * Configura la hora local basada en la ubicación y sincroniza `millis()`.
 */
void localTimeSetUp();

/**
 * Devuelve la hora actual ajustada según Epoch y millis().
 */
String getAdjustedTime();

/**
 * Muestra la hora ajustada en el LCD o Serial.
 */
void updateClockDisplay();

#endif