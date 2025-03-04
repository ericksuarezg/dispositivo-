#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "LcdSetup.h"
#include "timers.h"
#include "freertos/semphr.h"
#include <ArduinoJson.h>
#include "timeSetUp.h"

// Variables globales
static TickType_t lastSyncTicks = 0;
time_t baseTime = 0;
unsigned long millisAtSync = 0;
long utcOffsetInSeconds = -18000; // Zona horaria de Bogotá, UTC -5 horas (-18000 segundos)
String payload;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Cliente NTP

unsigned long currentTime = 0;
bool timeConfigured = false;


/* bool fetchUtcOffset() {
  HTTPClient http;
  http.begin("http://ip-api.com/json/"); // Consulta IP y zona horaria
  int httpCode = http.GET();

  if (httpCode == 200) {
    payload = http.getString();
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
} */
bool fetchUtcOffset() {
  // Crear instancia del cliente HTTP y definir la URL de la API
  HTTPClient http;
  http.begin("http://ip-api.com/json/");
  
  // Realizar la petición GET a la API
  int httpCode = http.GET();

  if (httpCode == 200) { // Si la respuesta es exitosa (HTTP 200)
    // Obtener la respuesta en formato String
    String payload = http.getString();
    Serial.println("Respuesta de la API:");
    Serial.println(payload);

    // Crear un documento JSON dinámico para parsear la respuesta
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print("Error al parsear JSON: ");
      Serial.println(error.c_str());
      http.end();
      return false;
    }

    // Extraer el campo "timezone" del JSON
    const char* timezone = doc["timezone"];
    if (timezone && strlen(timezone) > 0) {
      Serial.print("Zona horaria detectada: ");
      Serial.println(timezone);

      // Configurar la variable de entorno TZ y aplicar la configuración
      setenv("TZ", timezone, 1);
      tzset();

      // Configurar NTP usando un offset fijo (por ejemplo, -18000 segundos para UTC-5)
      // Nota: utcOffsetInSeconds debe estar declarado globalmente o definido previamente.
      configTime(-18000, 0, "pool.ntp.org", "time.nist.gov");

      http.end();
      return true;
    } else {
      Serial.println("Campo 'timezone' no encontrado o vacío en la respuesta.");
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
  if (timeConfigured || WiFi.status() != WL_CONNECTED){
    return;
  }
  
  if (!fetchUtcOffset()) {
    Serial.println("No se pudo obtener la zona horaria. Usando UTC.");
    setenv("TZ", "UTC", 1); // Usar UTC si no se obtiene la zona horaria
    tzset();
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  }
  
  // Verificar la sincronización de NTP
 /*  unsigned long startAttemptTime = millis();
  while (!timeClient.update()) {
    if (millis() - startAttemptTime > 5000) { // Timeout después de 5 segundos
      Serial.println("No se pudo sincronizar con el servidor NTP.");
      break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Esperar 1 segundo y reintentar
  } */
  uint64_t startAttemptTime =esp_timer_get_time();;  // Guardar el valor de millis() en uint64_t
  uint64_t currentMillis;
  
  while (!timeClient.update()) {
    currentMillis = esp_timer_get_time();;  // Obtener el tiempo actual con millis()
    // Comparar correctamente considerando el desbordamiento
    if ((currentMillis - startAttemptTime) > 5000000) {  // Timeout después de 5 segundos
        Serial.println("No se pudo sincronizar con el servidor NTP.");
        break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Esperar 1 segundo y reintentar
  }

  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    // Cambiar el formato para mostrar fecha y hora completas
    char timeString[20];  // Se aumenta el tamaño para fecha completa
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeInfo);
    Serial.print("Fecha y hora local sincronizada: ");
    Serial.println(timeString);

    baseTime = mktime(&timeInfo); // Hora y fecha inicial sincronizada
    millisAtSync = esp_timer_get_time();     // Guardar tiempo en millis()
    timeConfigured = true;
    //displayInfoOnLCD("Fecha y Hora Local:", timeString);  // Mostrar en la LCD
  } else {
    Serial.println("No se pudo sincronizar la hora local.");
    timeConfigured = false;
  }
  getAdjustedTime();
}

/* String getAdjustedTime() {
  if (baseTime == 0) {
    return "Hora no sincronizada";
  }

  // Obtener el tiempo actual con millis()
  unsigned long currentMillis = millis();

  // Detectar desbordamiento de millis()
  if (currentMillis < millisAtSync) {  
    millisAtSync = currentMillis;  // Si hay desbordamiento, reinicia millisAtSync
  }

  // Calcular tiempo ajustado usando millis()
  unsigned long elapsedMillis = millis() - millisAtSync;
  
  // Si el desajuste es demasiado grande, se vuelve a sincronizar
  if (elapsedMillis > 2000) {  // Aquí puedes poner el valor que desees para la tolerancia
    baseTime = time(nullptr);  // Sincroniza baseTime con la hora real
    millisAtSync = millis();   // Reinicia millisAtSync para corregir cualquier error acumulado
    Serial.println("Re-sincronizando hora...");  // Mensaje para saber que la hora se está re-sincronizando
  }
  
  
  
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
} */
String getAdjustedTime() {
  if (baseTime == 0) {
    return "Hora no sincronizada";
  }

  // Obtener el tiempo actual con xTaskGetTickCount()
  TickType_t currentTicks = xTaskGetTickCount();
  TickType_t elapsedTicks = currentTicks - lastSyncTicks;
  time_t elapsedSeconds = elapsedTicks / configTICK_RATE_HZ;

  // Si el desajuste es demasiado grande, re-sincronizar
  if (elapsedSeconds > 2000) {  
    baseTime = time(nullptr);  // Sincroniza baseTime con la hora real
    lastSyncTicks = xTaskGetTickCount();  // Reinicia lastSyncTicks
    Serial.println("Re-sincronizando hora...");
  }

  // Obtener el tiempo ajustado (baseTime + tiempo transcurrido)
  time_t adjustedTime = baseTime + elapsedSeconds;

  // Convertir el tiempo ajustado a una estructura de tiempo para formateo
  struct tm* adjustedTimeInfo = localtime(&adjustedTime);

  // Formato adecuado para la fecha y hora completa (Año-Mes-Día Hora:Minuto:Segundo)
  char timeString[30];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", adjustedTimeInfo);

  // Extraer solo la hora (Hora:Minuto:Segundo)
  char hourString[10];
  strftime(hourString, sizeof(hourString), "%H:%M:%S", adjustedTimeInfo);

  // Convertir hourString en time_t
  int hour, minute, second;
  sscanf(hourString, "%d:%d:%d", &hour, &minute, &second);

  // Copiar información existente del adjustedTimeInfo para no perder la fecha
  struct tm timeStruct = *adjustedTimeInfo;
  timeStruct.tm_hour = hour;
  timeStruct.tm_min = minute;
  timeStruct.tm_sec = second;

  // Convertir a time_t
  time_t hourAsTimeT = mktime(&timeStruct);
  startTimers(hourAsTimeT);

  return String(timeString);
}

void updateClockDisplay(SemaphoreHandle_t lcdSemaphore) {
  String currentTime = getAdjustedTime();
  Serial.print("Hora actual ajustada: ");
  Serial.println(currentTime);
  if (xSemaphoreTake(lcdSemaphore,5000/portTICK_PERIOD_MS)==pdTRUE){
    displayInfoOnLCD("Fecha       Hora", currentTime.c_str());
    vTaskDelay(pdMS_TO_TICKS(2000));
    xSemaphoreGive(lcdSemaphore);
  }
}

String getDateSeparate() {
    // Ejecutar la función pasada como parámetro y capturar su retorno
    String dataTime = getAdjustedTime();

    // Validar si la fecha y hora es válida
    if (dataTime == "Hora no sincronizada" || dataTime.isEmpty()) {
        return ("Sin fecha", "Sin hora"); // Devolver valores predeterminados
    }

    // Separar la fecha y la hora a partir del espacio
    int spaceIndex = dataTime.indexOf(' ');
    if (spaceIndex != -1) {
        String datePart = dataTime.substring(0, spaceIndex);       // Extraer la fecha
        return datePart;                              // Devolver como par
    }

    // Si el formato es inválido
    return ("Formato inválido", "Formato inválido");
}


String getTimeSeparate() {
    // Ejecutar la función pasada como parámetro y capturar su retorno
    String dataTime = getAdjustedTime();

    // Validar si la fecha y hora es válida
    if (dataTime == "Hora no sincronizada" || dataTime.isEmpty()) {
        return ("Sin fecha", "Sin hora"); // Devolver valores predeterminados
    }

    // Separar la fecha y la hora a partir del espacio
    int spaceIndex = dataTime.indexOf(' ');
    if (spaceIndex != -1) {
        String timePart = dataTime.substring(spaceIndex + 1);      // Extraer la hora
        return timePart;                              // Devolver como par
    }

    // Si el formato es inválido
    return ("Formato inválido", "Formato inválido");
}