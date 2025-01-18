#include <Arduino.h>
#include "timers.h"
#include "LcdSetUp.h"
#include "ds18b20SetUp.h"
#include "dthSetUp.h"
#include "timeSetUp.h"
#include "storage.h"
#include "mqtt.h"

// Declarar la variable como extern
extern SemaphoreHandle_t lcdSemaphore;  // Crear el semáforo
float temperatureCDs18b20 = NAN;  
float temperaturaDHT = NAN;
float humedadDHT = NAN;  // Variables para el sensor DHT22


// Definir los temporizadores globalesS
Ticker timerTask1;  // Para la tarea a las 3:00:00 PM
Ticker timerTask2;  // Para la tarea a las 4:30:00 PM
Ticker timerTask3;  // Para la tarea a las 5:15:00 PM

unsigned long timeAtStart;  // Marca del tiempo al inicio del programa

// Funciones que se ejecutarán a las horas específicas
void tareaProgramada1() {
  Serial.println("INICIO EJECUCION TAREA PROGRAMADA 1");
 
  Serial.println("TERMINANDO LA TAREA1 lectura de sensores a la 1:00:00 PM");
}

void tareaProgramada2() {
  Serial.println("Ejecutando tarea programada a las 11:57:00 AM");
}

void tareaProgramada3() {
  Serial.println("Ejecutando tarea programada a las 11:57:05 AM");
}

// Función para convertir hora, minuto, y segundo a milisegundos desde el inicio del día
unsigned long convertTimeToMillis(int targetHour, int targetMinute, int targetSecond) {
  return (targetHour * 3600L + targetMinute * 60 + targetSecond) * 1000;  // Convertir a milisegundos
}

// Funciones que encapsulan las tareas programadas, sin usar lambdas
void tareaProgramada1Function() {
  tareaProgramada1();  // Llamar a la tarea que ejecuta la tarea 1
}


void tareaProgramada2Function() {
  tareaProgramada2();  // Llamar a la tarea que ejecuta la tarea 2
}

void tareaProgramada3Function() {
  tareaProgramada3();  // Llamar a la tarea que ejecuta la tarea 3
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
  unsigned long timeTo6_10PM = (11 * 3600) + (00 * 60); // 14:46:00
  unsigned long timeTo6_15PM = (12 * 3600) + (32 * 60); // 14:48:00
  unsigned long timeTo6_20PM = (12 * 3600) + (54 * 60); // 14:50:00

  // Reprogramar tareas basadas en el tiempo actual
  unsigned long remainingTime;
  if (secondsFromStartOfDay < timeTo6_10PM) {
    remainingTime = timeTo6_10PM - secondsFromStartOfDay;
    timerTask1.once(remainingTime, tareaProgramada1Function);
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
