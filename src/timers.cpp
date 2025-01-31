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
Ticker timerTask1;  // Para la tarea a las 3:00 AM
Ticker timerTask2;  // Para la tarea a las 09:00 AM
Ticker timerTask3;  // Para la tarea a las 03:00 PM
Ticker timerTask4;  // Para la tarea a las 09:00 PM


unsigned long timeAtStart;  // Marca del tiempo al inicio del programa


// Funciones que se ejecutarán a las horas específicas
void tareaProgramada1() {
  Serial.println("----------------------------------------");
    Serial.println("Ejecutando tarea programada a las 3:00:00 AM");
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
      alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("------------------------");
    timerTask1.once(24*60*60, tareaProgramada1);
    Serial.println("Tarea reprogramada para mañana a las 03:00:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada2() {
  Serial.println("Ejecutando tarea programada a las 09:00:00 AM");
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
      alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask2.once(24*60*60, tareaProgramada2);
    Serial.println("Tarea reprogramada para mañana a las 09:00:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada3() {
  Serial.println("Ejecutando tarea programada a las 03:00:00 PM");
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
      alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask3.once(24*60*60, tareaProgramada3);
    Serial.println("Tarea reprogramada para mañana a las 03:00:00 PM");
    Serial.println("----------------------------------------");
}


void tareaProgramada4() {
  Serial.println("Ejecutando tarea programada a las 09:00:00 PM");
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
      alertaOutSensorService("DS18B20", "sensor_failure");
      return;
    }

    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask4.once(24*60*60, tareaProgramada4);
    Serial.println("Tarea reprogramada para mañana a las 09:00:00 PM");
    Serial.println("----------------------------------------");
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
    unsigned long timeTo3AM = (3 * 3600) + (00 * 60);   // 03:00:00 (3:00 AM)
    unsigned long timeTo9AM = (9 * 3600) + (00 * 60); // 09:00:00 (9:00 AM)
    unsigned long timeTo3PM = (15 * 3600) + (00 * 60); // 03:00:00 (3:00 PM)
    unsigned long timeTo9PM = (21 * 3600) + (00 * 60); // 09:00:00 (9:00 PM)


  // Lógica de reprogramación, ajustada para tareas del mismo día o día siguiente
  unsigned long remainingTime;
  unsigned long secondsInADay = 24 * 3600;

  // Programar Tarea 1 (03:00 AM)
    if (secondsFromStartOfDay < timeTo3AM) {
        remainingTime = timeTo3AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo3AM;
    }
    timerTask1.once(remainingTime, tareaProgramada1);
    Serial.printf("Tarea 1 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 2 (9:00 AM)
    if (secondsFromStartOfDay < timeTo9AM) {
        remainingTime = timeTo9AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo9AM;
    }
    timerTask2.once(remainingTime, tareaProgramada2);
    Serial.printf("Tarea 2 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 3 (3:00 PM)
    if (secondsFromStartOfDay < timeTo3PM) {
        remainingTime = timeTo3PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo3PM;
    }
    timerTask3.once(remainingTime, tareaProgramada3);
    Serial.printf("Tarea 3 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 4 (9:00 PM)
    if (secondsFromStartOfDay < timeTo9PM) {
        remainingTime = timeTo9PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo9PM;
    }
    timerTask4.once(remainingTime, tareaProgramada4);
    Serial.printf("Tarea 3 programada para ejecutarse en %lu segundos\n", remainingTime);
}