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
#include <SPIFFS.h>
//#include <Ticker.h>

bool initTimers= false;
int lastProgrammedDay = -1; // Guarda el último día en el que se programaron tareas


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
Ticker timerTask1;  // Para la tarea a las 5:06:00 AM
Ticker timerTask2;  // Para la tarea a las 4:30:00 PM
Ticker timerTask3;  // Para la tarea a las 5:15:00 PM

unsigned long timeAtStart;  // Marca del tiempo al inicio del programa


// Funciones que se ejecutarán a las horas específicas
void tareaProgramada1() {
  Serial.println("----------------------------------------");
    Serial.println("Ejecutando tarea programada a las 05:06:00 AM");
    Serial.print("Hora actual: ");
    Serial.println(getTimeSeparate());;
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
      //alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
     // alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    //verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
    }
    Serial.println("------------------------");
    timerTask1.once(24*60*60, tareaProgramada1);
    Serial.println("Tarea reprogramada para mañana a las 05:06:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada2() {
  Serial.println("Ejecutando tarea programada a las 05:09 AM");
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
      //alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      //alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    //verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
    }
    Serial.println("----------------------------------------");
    timerTask2.once(24*60*60, tareaProgramada2);
    Serial.println("Tarea reprogramada para mañana a las 5:09:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada3() {
  Serial.println("Ejecutando tarea programada a las 11:10 AM");
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
      //alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      //alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    //verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
    }
    Serial.println("----------------------------------------");
    timerTask3.once(24*60*60, tareaProgramada3);
    Serial.println("Tarea reprogramada para mañana a las 11:11:00 AM");
    Serial.println("----------------------------------------");
}


// Función que ajusta la tarea programada en base al tiempo ajustado (ahora con un parámetro)
void startTimers(time_t adjustedTime) {
  struct tm* timeInfo = localtime(&adjustedTime);
  int baseHour = timeInfo->tm_hour;
  int baseMinute = timeInfo->tm_min;
  int baseSecond = timeInfo->tm_sec;

  int currentDay = timeInfo->tm_mday; // Obtener el día del mes actual
  Serial.printf("dia de hoy: ");
  Serial.print(currentDay);
  Serial.printf("dia configurado: ");
  Serial.print(lastProgrammedDay);

  if (initTimers && lastProgrammedDay == currentDay) {
    Serial.print("los temporizadores ya fueron inicializados hoy");
    return;
  }
  

  Serial.print("Hora actual: ");
  Serial.printf("%02d:%02d:%02d\n", baseHour, baseMinute, baseSecond);

  // Tiempo actual en segundos desde el inicio del día
  unsigned long secondsFromStartOfDay = (baseHour * 3600) + (baseMinute * 60) + baseSecond;

  // Tiempos programados en segundos desde el inicio del día
    unsigned long timeTo10PM = (8 * 3600) + (30 * 60);   // 22:00:00 (10:00 PM)
    unsigned long timeTo5_15AM = (16 * 3600) + (30 * 60); // 05:15:00 (5:15 AM)
    unsigned long timeTo8_10AM = (20 * 3600) + (30 * 60); // 08:10:00 (8:10 AM)

  // Lógica de reprogramación, ajustada para tareas del mismo día o día siguiente
  unsigned long remainingTime;
  unsigned long secondsInADay = 24 * 3600;

  // Programar Tarea 1 (10:00 PM)
    if (secondsFromStartOfDay < timeTo10PM) {
        remainingTime = timeTo10PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo10PM;
    }
    timerTask1.once(remainingTime, tareaProgramada1);
    Serial.printf("Tarea 1 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 2 (5:15 AM)
    if (secondsFromStartOfDay < timeTo5_15AM) {
        remainingTime = timeTo5_15AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo5_15AM;
    }
    timerTask2.once(remainingTime, tareaProgramada2);
    Serial.printf("Tarea 2 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 3 (8:10 AM)
    if (secondsFromStartOfDay < timeTo8_10AM) {
        remainingTime = timeTo8_10AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo8_10AM;
    }
    timerTask3.once(remainingTime, tareaProgramada3);
    Serial.printf("Tarea 3 programada para ejecutarse en %lu segundos\n", remainingTime);
    lastProgrammedDay = currentDay; // Actualizar el día programado
    initTimers=true;
} 
