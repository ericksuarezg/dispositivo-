#include <Arduino.h>
#include "timers.h"
#include "LcdSetUp.h"
#include "dthSetUp.h"
#include "alert.h"
#include "timeSetUp.h"
#include "storage.h"
#include "mqtt.h"
#include "WiFiManager.h"
#include <SPIFFS.h>


float getTemperaturaDHT() {
  return NAN;
}

float getHumedad() {
  return NAN;
}

// Definir los temporizadores globales
Ticker timerTask1;  // Para la tarea a las 2:13 AM
Ticker timerTask2;  // Para la tarea a las 5:41 AM
Ticker timerTask3;  // Para la tarea a las 8:15 AM
Ticker timerTask4;  // Para la tarea a las 11:38 AM
Ticker timerTask5;  // Para la tarea a las 3:15 PM
Ticker timerTask6;  // Para la tarea a las 6:36 PM
Ticker timerTask7;  // Para la tarea a las 09:05 PM
Ticker timerTask8;  // Para la tarea a las 10:53 PM
Ticker timerTask9;  // Para la tarea a las 11:56 PM


unsigned long timeAtStart;  // Marca del tiempo al inicio del programa


// Funciones que se ejecutarán a las horas específicas
void tareaProgramada1() {
  Serial.println("----------------------------------------");
    Serial.println("Ejecutando tarea programada a las 2:13:00 AM");
    Serial.print("Hora actual: ");
    Serial.println(getTimeSeparate());;
    float temperatureCDs18b20;
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, 1);
    }
    Serial.println("------------------------");
    timerTask1.once(24*60*60, tareaProgramada1);
    Serial.println("Tarea reprogramada para mañana a las 2:13:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada2() {
  Serial.println("Ejecutando tarea programada a las 5:41:00 AM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, 1);
    }
    Serial.println("----------------------------------------");
    timerTask2.once(24*60*60, tareaProgramada2);
    Serial.println("Tarea reprogramada para mañana a las 5:41:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada3() {
  Serial.println("Ejecutando tarea programada a las 08:15:00 AM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask3.once(24*60*60, tareaProgramada3);
    Serial.println("Tarea reprogramada para mañana a las 08:15:00 AM");
    Serial.println("----------------------------------------");
}


void tareaProgramada4() {
  Serial.println("Ejecutando tarea programada a las 11:38:00 AM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask4.once(24*60*60, tareaProgramada4);
    Serial.println("Tarea reprogramada para mañana a las 11:38:00 AM");
    Serial.println("----------------------------------------");
}

void tareaProgramada5() {
  Serial.println("Ejecutando tarea programada a las 3:15:00 PM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask5.once(24*60*60, tareaProgramada5);
    Serial.println("Tarea reprogramada para mañana a las 3:15:00 PM");
    Serial.println("----------------------------------------");
}

void tareaProgramada6() {
  Serial.println("Ejecutando tarea programada a las6:36:00 PM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask6.once(24*60*60, tareaProgramada6);
    Serial.println("Tarea reprogramada para mañana a las 6:36:00 PM");
    Serial.println("----------------------------------------");
}

void tareaProgramada7() {
  Serial.println("Ejecutando tarea programada a las 9:05:00 PM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask7.once(24*60*60, tareaProgramada7);
    Serial.println("Tarea reprogramada para mañana a las 9:05:00 PM");
    Serial.println("----------------------------------------");
}

void tareaProgramada8() {
  Serial.println("Ejecutando tarea programada a las 10:53:00 PM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask8.once(24*60*60, tareaProgramada8);
    Serial.println("Tarea reprogramada para mañana a las 10:53:00 PM");
    Serial.println("----------------------------------------");
}

void tareaProgramada9() {
  Serial.println("Ejecutando tarea programada a las 11:56:00 PM");
    float temperaturaDHT;
    float humedad;
    //trae la temperatura del ds18b20 y del dht
    temperaturaDHT = dhtGetTemperature();
    humedad = dhtGetHumidity();

    // Verificar si los datos son validos
    if(isnan(temperaturaDHT) || isnan(humedad)){
      Serial.println("Sensor DHT no encontrado");
      alertaOutSensorService("DHT", "sensor_failure");
      return;
    }

    // Verificar alertas
    verificarAlertas(temperaturaDHT, humedad);
    // Verificar conexión WiFi y MQTT antes de publicar
    if (WiFi.status() == WL_CONNECTED && isMQTTConnected()) {
        publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
        Serial.println("Datos publicados exitosamente");
    } else {
        Serial.println("Sin conexión - Guardando datos para envío posterior");
        //saveDataToCSV("", getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 1);
    }
    Serial.println("----------------------------------------");
    timerTask9.once(24*60*60, tareaProgramada9);
    Serial.println("Tarea reprogramada para mañana a las 11:56:00 PM");
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
    unsigned long timeTo2_13AM= (2 * 3600) + (12 * 60);   // 2:13:00 (2:13 AM)
    unsigned long timeTo5_41AM = (5 * 3600) + (41 * 60); // 5:41:00 (5:41 AM)
    unsigned long timeTo8_15AM = (8 * 3600) + (15 * 60); // 8:15:00 (8:15 AM)
    unsigned long timeTo11_38AM = (11 * 3600) + (38 * 60); // 11:38:00 (11:38 AM)
    unsigned long timeTo3_15PM = (15 * 3600) + (15 * 60); // 3:15:00 (3:15 PM)
    unsigned long timeTo6_36PM = (18 * 3600) + (36 * 60); // 18:36:00 (6:36 PM)
    unsigned long timeTo9_05PM = (21 * 3600) + (05 * 60); // 9:05:00 (9:05 PM)
    unsigned long timeTo10_53PM = (10 * 3600) + (53 * 60); // 10:53:00 (10:53 PM)
    unsigned long timeTo11_56PM = (11 * 3600) + (56 * 60); // 11:56:00 (11:56 AM)



  // Lógica de reprogramación, ajustada para tareas del mismo día o día siguiente
  unsigned long remainingTime;
  unsigned long secondsInADay = 24 * 3600;

  // Programar Tarea 1 (2:13 AM)
    if (secondsFromStartOfDay < timeTo2_13AM) {
        remainingTime = timeTo2_13AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo2_13AM;
    }
    timerTask1.once(remainingTime, tareaProgramada1);
    Serial.printf("Tarea 1 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 2 (5:41 AM)
    if (secondsFromStartOfDay < timeTo5_41AM) {
        remainingTime = timeTo5_41AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo5_41AM;
    }
    timerTask2.once(remainingTime, tareaProgramada2);
    Serial.printf("Tarea 2 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 3 (8:15 PM)
    if (secondsFromStartOfDay < timeTo8_15AM) {
        remainingTime = timeTo8_15AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo8_15AM;
    }
    timerTask3.once(remainingTime, tareaProgramada3);
    Serial.printf("Tarea 3 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 4 (11:38 AM)
    if (secondsFromStartOfDay < timeTo11_38AM) {
        remainingTime = timeTo11_38AM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo11_38AM;
    }
    timerTask4.once(remainingTime, tareaProgramada4);
    Serial.printf("Tarea 4 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 5 (3:15 PM)
    if (secondsFromStartOfDay < timeTo3_15PM) {
        remainingTime = timeTo3_15PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo3_15PM;
    }
    timerTask5.once(remainingTime, tareaProgramada5);
    Serial.printf("Tarea 5 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 6 (6:36 PM)
    if (secondsFromStartOfDay < timeTo6_36PM) {
        remainingTime = timeTo6_36PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo6_36PM;
    }
    timerTask6.once(remainingTime, tareaProgramada6);
    Serial.printf("Tarea 6 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 7 (9:05 PM)
    if (secondsFromStartOfDay < timeTo9_05PM) {
        remainingTime = timeTo9_05PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo9_05PM;
    }
    timerTask7.once(remainingTime, tareaProgramada7);
    Serial.printf("Tarea 7 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 8 (10:53 PM)
    if (secondsFromStartOfDay < timeTo10_53PM) {
        remainingTime = timeTo10_53PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo10_53PM;
    }
    timerTask8.once(remainingTime, tareaProgramada8);
    Serial.printf("Tarea 8 programada para ejecutarse en %lu segundos\n", remainingTime);

    // Programar Tarea 9 (11:56 PM)
    if (secondsFromStartOfDay < timeTo11_56PM) {
        remainingTime = timeTo11_56PM - secondsFromStartOfDay;
    } else {
        remainingTime = secondsInADay - secondsFromStartOfDay + timeTo11_56PM;
    }
    timerTask9.once(remainingTime, tareaProgramada9);
    Serial.printf("Tarea 9 programada para ejecutarse en %lu segundos\n", remainingTime);
}