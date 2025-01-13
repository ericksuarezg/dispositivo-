#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "LcdSetup.h"
#include "timers.h"

// Variables globales
time_t baseTime = 0;
unsigned long millisAtSync = 0;
long utcOffsetInSeconds = -18000; // Zona horaria de Bogotá, UTC -5 horas (-18000 segundos)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Cliente NTP

unsigned long currentTime = 0;


bool fetchUtcOffset() {
  HTTPClient http;
  http.begin("http://ip-api.com/json/"); // Consulta IP y zona horaria
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Respuesta de la API:");
    Serial.println(payload);

    int tzIndex = payload.indexOf("\"timezone\":\"");
    if (tzIndex != -1) {
      int start = tzIndex + 12;
      int end = payload.indexOf("\"", start);
      String timezone = payload.substring(start, end);

      Serial.print("Zona horaria detectada: ");
      Serial.println(timezone);

      // Configurar el entorno de tiempo basado en la zona horaria
      setenv("TZ", timezone.c_str(), 1);
      tzset(); // Aplicar configuración de zona horaria

      // Calcular el UTC Offset
      utcOffsetInSeconds = -18000;  // UTC -5 horas (Bogotá)

      // Configurar NTP con el UTC Offset
      configTime(utcOffsetInSeconds, 0, "pool.ntp.org", "time.nist.gov");
      return true;
    } else {
      Serial.println("No se encontró el campo 'timezone' en la respuesta de la API.");
    }
  } else {
    Serial.print("Error al obtener la ubicación. Código HTTP: ");
    Serial.println(httpCode);
  }

  http.end();
  return false;
}

void localTimeSetUp() {
  // Sincronizar NTP con la zona horaria
  if (!fetchUtcOffset()) {
    Serial.println("No se pudo obtener la zona horaria. Usando UTC.");
    setenv("TZ", "UTC", 1); // Usar UTC si no se obtiene la zona horaria
    tzset();
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  }
  
  // Verificar la sincronización de NTP
  unsigned long startAttemptTime = millis();
  while (!timeClient.update()) {
    if (millis() - startAttemptTime > 5000) { // Timeout después de 5 segundos
      Serial.println("No se pudo sincronizar con el servidor NTP.");
      break;
    }
    delay(1000); // Esperar 1 segundo y reintentar
  }

  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    // Cambiar el formato para mostrar fecha y hora completas
    char timeString[20];  // Se aumenta el tamaño para fecha completa
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
    Serial.print("Fecha y hora local sincronizada: ");
    Serial.println(timeString);

    baseTime = mktime(&timeInfo); // Hora y fecha inicial sincronizada
    millisAtSync = millis();     // Guardar tiempo en millis()

    displayInfoOnLCD("Fecha y Hora Local:", timeString);  // Mostrar en la LCD
  } else {
    Serial.println("No se pudo sincronizar la hora local.");
  }
}

String getAdjustedTime() {
  if (baseTime == 0) {
    return "Hora no sincronizada";
  }


  // Calcular tiempo ajustado usando millis()
  unsigned long elapsedMillis = millis() - millisAtSync;
  
  // Obtener el tiempo ajustado (baseTime + tiempo transcurrido)
  time_t adjustedTime = baseTime + (elapsedMillis / 1000);  // Añadir segundos al tiempo base

  // Convertir el tiempo ajustado a una estructura de tiempo para formateo
  struct tm* adjustedTimeInfo = localtime(&adjustedTime);  // Convierte a estructura tm

  // Formato adecuado para la fecha y hora completa (Año-Mes-Día Hora:Minuto:Segundo)
  char timeString[30];  // Tamaño suficiente para almacenar la fecha completa
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", adjustedTimeInfo);

  // Extraer solo la hora (Hora:Minuto:Segundo)
  char hourString[10];  // Tamaño suficiente para la hora en formato HH:MM:SS
  strftime(hourString, sizeof(hourString), "%H:%M:%S", adjustedTimeInfo);

  // Convertir hourString en time_t
  int hour, minute, second;
  sscanf(hourString, "%d:%d:%d", &hour, &minute, &second);  // Extraer horas, minutos y segundos

  // Copiar información existente del adjustedTimeInfo para no perder la fecha
  struct tm timeStruct = *adjustedTimeInfo;  // Clonamos adjustedTimeInfo
  timeStruct.tm_hour = hour;
  timeStruct.tm_min = minute;
  timeStruct.tm_sec = second;

  // Convertir a time_t
  time_t hourAsTimeT = mktime(&timeStruct);
  
  //startTimers(hourString);
  startTimers(hourAsTimeT);
  // Retornar la fecha y hora en formato YYYY-MM-DD HH:MM:SS
  return String(timeString);
}

void updateClockDisplay() {
  String currentTime = getAdjustedTime();
  Serial.print("Hora actual ajustada: ");
  Serial.println(currentTime);
  displayInfoOnLCD("Hora:", currentTime.c_str());
}