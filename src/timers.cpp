#include <Arduino.h>
#include "timers.h"
#include "LcdSetUp.h"
#include "ds18b20SetUp.h"
#include "dthSetUp.h"
#include "alert.h"
#include "timeSetUp.h"
#include "storage.h"
#include "mqtt.h"
#include "WiFiManager.h"


float getTemperatureCDs18b20() {
  return NAN;
}

float getTemperaturaDHT() {
  return NAN;
}

float getHumedad() {
  return NAN;
}

// Definir los temporizadores globales
Ticker timerTask1;  // Para la tarea a las 3:00:00 PM
Ticker timerTask2;  // Para la tarea a las 4:30:00 PM
Ticker timerTask3;  // Para la tarea a las 5:15:00 PM

unsigned long timeAtStart;  // Marca del tiempo al inicio del programa


// Funciones que se ejecutarán a las horas específicas
void tareaProgramada1() {
  Serial.println("Ejecutando tarea programada a las 1000 PM");
    float temperatureCDs18b20;
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperatureCDs18b20 = ds18b20GetTemperature();
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(temperatureCDs18b20 == -127 || (isnan(temperatureCDs18b20))){
      Serial.println("Sensor DS18B20 no encontrado");
      alertaOutSensorService("DS18B20", "Sensor no encontrado");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "Sensor no encontrado");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);

}

void tareaProgramada2() {
  Serial.println("Ejecutando tarea programada a las 5:15 AM");
    float temperatureCDs18b20;
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperatureCDs18b20 = ds18b20GetTemperature();
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(temperatureCDs18b20 == -127 || (isnan(temperatureCDs18b20))){
      Serial.println("Sensor DS18B20 no encontrado");
      alertaOutSensorService("DS18B20", "Sensor no encontrado");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "Sensor no encontrado");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
}

void tareaProgramada3() {
  Serial.println("Ejecutando tarea programada a las 8:10 AM");
    float temperatureCDs18b20;
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperatureCDs18b20 = ds18b20GetTemperature();
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(temperatureCDs18b20 == -127 || (isnan(temperatureCDs18b20))){
      Serial.println("Sensor DS18B20 no encontrado");
      alertaOutSensorService("DS18B20", "Sensor no encontrado");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "Sensor no encontrado");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);

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
  unsigned long timeTo6_10PM = (22 * 3600) + (00 * 60);  // 17:00:00 (5:00 PM)
  unsigned long timeTo6_15PM = (5 * 3600) + (15 * 60); // 17:15:00 (5:15 PM)
  unsigned long timeTo6_20PM = (8 * 3600) + (10 * 60); // 17:20:00 (5:20 PM)

  // Lógica de reprogramación, ajustada para tareas del mismo día o día siguiente
  unsigned long remainingTime;

  // Tarea 1: se ejecuta a las 5:00 PM (17:00:00)
  if (secondsFromStartOfDay < timeTo6_10PM) {
    remainingTime = timeTo6_10PM - secondsFromStartOfDay;
    timerTask1.once(remainingTime, tareaProgramada1Function);
    Serial.print("Tarea programada 1 en: ");
    Serial.println(remainingTime);
  }
  // Tarea 2: se ejecuta a las 5:15 PM (17:15:00)
  else if (secondsFromStartOfDay < timeTo6_15PM) {
    remainingTime = timeTo6_15PM - secondsFromStartOfDay;
    timerTask2.once(remainingTime, tareaProgramada2Function);
    Serial.print("Tarea programada 2 en: ");
    Serial.println(remainingTime);
  }
  // Tarea 3: se ejecuta a las 5:20 PM (17:20:00)
  else if (secondsFromStartOfDay < timeTo6_20PM) {
    remainingTime = timeTo6_20PM - secondsFromStartOfDay;
    timerTask3.once(remainingTime, tareaProgramada3Function);
    Serial.print("Tarea programada 3 en: ");
    Serial.println(remainingTime);
  } else {
    // Si ya pasó el tiempo de las tres tareas, reprogramarlas para el día siguiente
    unsigned long secondsInADay = 24 * 3600; // 24 horas en segundos

    remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_10PM;  // Día siguiente
    timerTask1.once(remainingTime, tareaProgramada1Function);
    Serial.print("Tarea programada 1 para el día siguiente en: ");
    Serial.println(remainingTime);

    remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_15PM;
    timerTask2.once(remainingTime, tareaProgramada2Function);
    Serial.print("Tarea programada 2 para el día siguiente en: ");
    Serial.println(remainingTime);

    remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_20PM;
    timerTask3.once(remainingTime, tareaProgramada3Function);
    Serial.print("Tarea programada 3 para el día siguiente en: ");
    Serial.println(remainingTime);
  }
}