#include "timers.h"
#include <Arduino.h>

// Definir los temporizadores globales
Ticker timerTask1;  // Para la tarea a las 3:00:00 PM
Ticker timerTask2;  // Para la tarea a las 4:30:00 PM
Ticker timerTask3;  // Para la tarea a las 5:15:00 PM

unsigned long timeAtStart;  // Marca del tiempo al inicio del programa

// Funciones que se ejecutarán a las horas específicas
void tareaProgramada1() {
  Serial.println("Ejecutando tarea programada a las 12:35:00 PM");
}

void tareaProgramada2() {
  Serial.println("Ejecutando tarea programada a las 12:37:00 PM");
}

void tareaProgramada3() {
  Serial.println("Ejecutando tarea programada a las 12:39:05 PM");
}

// Función para convertir hora, minuto, y segundo a milisegundos desde el inicio del día
unsigned long convertTimeToMillis(int targetHour, int targetMinute, int targetSecond) {
  return (targetHour * 3600L + targetMinute * 60 + targetSecond) * 1000;  // Convertir a milisegundos
}

// Función que ajusta la tarea programada en base al tiempo ajustado (ahora con un parámetro)
void startTimers(time_t adjustedTime) {
  struct tm* timeInfo = localtime(&adjustedTime);

  int baseHour = timeInfo->tm_hour;
  int baseMinute = timeInfo->tm_min;
  int baseSecond = timeInfo->tm_sec;

  Serial.print("Hora actual: ");
  Serial.printf("%02d:%02d:%02d\n", baseHour, baseMinute, baseSecond);

  // Tiempo actual en segundos desde el inicio del día
  unsigned long secondsFromStartOfDay = (baseHour * 3600) + (baseMinute * 60) + baseSecond;

  // Tiempos programados en segundos desde el inicio del día
  unsigned long timeTo6_10PM = (12 * 3600) + (35 * 60); // 18:10:00
  unsigned long timeTo6_15PM = (12 * 3600) + (37 * 60); // 18:15:00
  unsigned long timeTo6_20PM = (12 * 3600) + (39 * 60); // 18:20:00

  // Reprogramar tareas basadas en el tiempo actual
  unsigned long remainingTime;
  if (secondsFromStartOfDay < timeTo6_10PM) {
    remainingTime = timeTo6_10PM - secondsFromStartOfDay;
    timerTask1.once(remainingTime, tareaProgramada1);
    Serial.print("Tarea programada 1 en: ");
    Serial.println(remainingTime);
  } else if (secondsFromStartOfDay < timeTo6_15PM) {
    remainingTime = timeTo6_15PM - secondsFromStartOfDay;
    timerTask2.once(remainingTime, tareaProgramada2);
    Serial.print("Tarea programada 2 en: ");
    Serial.println(remainingTime);
  } else if (secondsFromStartOfDay < timeTo6_20PM) {
    remainingTime = timeTo6_20PM - secondsFromStartOfDay;
    timerTask3.once(remainingTime, tareaProgramada3);
    Serial.print("Tarea programada 3 en: ");
    Serial.println(remainingTime);
  } else {
    // Si ya pasó el tiempo programado, reprogramar las tareas para el día siguiente
    unsigned long secondsInADay = 24 * 3600;

    remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_10PM;
    timerTask1.once(remainingTime, tareaProgramada1);
    Serial.print("Tarea programada 1 para el día siguiente en: ");
    Serial.println(remainingTime);

    remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_15PM;
    timerTask2.once(remainingTime, tareaProgramada2);
    Serial.print("Tarea programada 2 para el día siguiente en: ");
    Serial.println(remainingTime);

    remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_20PM;
    timerTask3.once(remainingTime, tareaProgramada3);
    Serial.print("Tarea programada 3 para el día siguiente en: ");
    Serial.println(remainingTime);
  }
}
