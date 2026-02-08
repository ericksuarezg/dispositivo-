#include "storage.h"
//#include <SPIFFS.h>
#include <WiFi.h>
#include <mqtt.h>
#include <LcdSetUp.h>
#include <ArduinoJson.h>
#include <wifiManagerSetUp.h>
#include "freertos/semphr.h"
#include "PublishingInit.h"
#include "LittleFS.h"

#include <vector>


void setupSPIFFS() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (!LittleFS.begin(true)) {
        Serial.println("Error inicializando SPIFFS");
        return;
    }
    Serial.println("LittleFS Configurado exitosamente.");

}

/* void saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20, int toSend) {
    
    if (isnan(tempDHT) || isnan(humedad) || isnan(tempDS18B20) || tempDS18B20==-127) {
        Serial.println("Error: Datos inválidos. No se almacenara información.");
        return; 
    }
  
    // Parsear el JSON del payload
    DynamicJsonDocument jsonDoc(512);
    DeserializationError error = deserializeJson(jsonDoc, payload);
    if (error) {
        Serial.print(F("Error al parsear el payload JSON: "));
        Serial.println(error.c_str());
        return;
    }

    // Extraer campos específicos del payload JSON
    String country = jsonDoc["country"] | "N/A";
    String regionName = jsonDoc["regionName"] | "N/A";
    String city = jsonDoc["city"] | "N/A";
    int zipCode = jsonDoc["zip"].as<int>();
    float latitude = jsonDoc["lat"].as<float>();
    float longitude = jsonDoc["lon"].as<float>();
    String timeZone = jsonDoc["timezone"] | "N/A";
    String isp = jsonDoc["isp"] | "N/A";
    String org = jsonDoc["org"] | "N/A";
    String as = jsonDoc["as"] | "N/A";

    // Abrir el archivo CSV
    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_APPEND);
    if (!file) {
        Serial.println("Error abriendo archivo CSV.");    
        return;
    }

    // Crear línea de datos para el CSV con las banderas toSend y sent
    String dataLine = country + "," + regionName + "," + city + "," + zipCode + "," + 
                     latitude + "," + longitude + "," + timeZone + "," + isp + "," + 
                     org + "," + as + "," + datePart + "," + timePart + "," + 
                     String(tempDHT) + "," + String(humedad) + "," + String(tempDS18B20) + "," + 
                     String(toSend) + ",0"; // sent=0 indica que no se ha enviado aún

    file.println(dataLine);
    file.close();
    Serial.println("Datos guardados en CSV: " + dataLine);
}
 */

/* void saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20, int toSend) {
    // Validar datos de los sensores
    if (isnan(tempDHT) || isnan(humedad) || isnan(tempDS18B20) || tempDS18B20 == -127) {
        Serial.println("Error: Datos inválidos. No se almacenará información.");
        return;
    }
    if (xSemaphoreTake(spiffsMutex, pdMS_TO_TICKS(200)) != pdTRUE) {
        Serial.println("No se pudo tomar mutex SPIFFS");
        return;
    }

    // Abrir el archivo CSV en modo añadir
    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_APPEND);
    if (!file) {
        Serial.println("Error abriendo archivo CSV.");
        xSemaphoreGive(spiffsMutex);    
        return;
    }

    // Crear línea de datos reducida
    String dataLine = datePart + "," + timePart + "," + String(tempDHT) + "," + String(humedad) + "," + String(tempDS18B20) + ",0";
    
    // Guardar en archivo y cerrar
    file.println(dataLine);
    file.close();
    xSemaphoreGive(spiffsMutex);
    Serial.println("Datos guardados en CSV: " + dataLine);
} */
bool saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20,int sequence) {
    // 1️⃣ Validación de datos
    if (isnan(tempDHT) || isnan(humedad) || isnan(tempDS18B20) || tempDS18B20 == -127) {
        Serial.println("Datos inválidos");
        return false;
    }
    
    // 2️⃣ Tomar mutex (bloqueante, porque el dato es crítico)
    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }

    // 3️⃣ Abrir archivo
    File file = LittleFS.open("/deviceDataSensor.csv", FILE_APPEND);
    if (!file) {
        xSemaphoreGive(spiffsMutex);
        return false;
    }

    // 4️⃣ Construir línea CSV INMUTABLE
    // fecha,hora,tempDHT,humedad,tempDS18,sequence
    String dataLine =
        datePart + "," +
        timePart + "," +
        String(tempDHT) + "," +
        String(humedad) + "," +
        String(tempDS18B20) + "," +
        String(sequence) + "," + "0";  // 👈 aquí vive la sequence

    bool ok = file.println(dataLine);

    // 5️⃣ Cerrar y liberar
    file.close();
    xSemaphoreGive(spiffsMutex);

    return ok;
}



/* void sendStoredData() {
    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_READ);
    if (!file) {
        Serial.println("No hay datos almacenados para enviar.");
        return;
    }
    
    std::vector<String> updatedLines;
    bool dataModified = false;

    // Leer el archivo línea por línea
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim(); // Eliminar espacios y saltos de línea
        
        if (line.length() == 0) continue;

        // Verificar formato correcto de la línea
        int commaCount = std::count(line.begin(), line.end(), ',');
        if (commaCount != 16) {
            Serial.println("Línea con formato incorrecto: " + line);
            updatedLines.push_back(line); // Mantener líneas con formato incorrecto
            continue;
        }

        // Extraer las banderas toSend y sent
        int lastComma = line.lastIndexOf(',');
        int secondLastComma = line.lastIndexOf(',', lastComma - 1);
        
        int sent = line.substring(lastComma + 1).toInt();
        int toSend = line.substring(secondLastComma + 1, lastComma).toInt();

        //Serial.println("Estado actual - toSend: " + String(toSend) + ", sent: " + String(sent));

        if (toSend == 1 && sent == 0 && isMQTTConnected()) {
            // Extraer los datos necesarios para enviar
            int startDataIndex = line.lastIndexOf(',', secondLastComma - 1);
            for (int i = 0; i < 3; i++) {
                startDataIndex = line.lastIndexOf(',', startDataIndex - 1);
            }
            
            String datePart = line.substring(line.lastIndexOf(',', startDataIndex - 1) + 1, line.indexOf(',', line.lastIndexOf(',', startDataIndex - 1) + 1));
            String timePart = line.substring(line.lastIndexOf(',', startDataIndex) + 1, line.indexOf(',', line.lastIndexOf(',', startDataIndex) + 1));
            int tempDHTStart = line.indexOf(',', startDataIndex + timePart.length()) + 1;
            int tempDHTEnd = line.indexOf(',', tempDHTStart);
            float tempDHT = line.substring(tempDHTStart, tempDHTEnd).toFloat();
            int humedadStart = line.indexOf(',', tempDHTEnd) + 1;
            int humedadEnd = line.indexOf(',', humedadStart);
            float humedad = line.substring(humedadStart, humedadEnd).toFloat();
            Serial.println("humedad: " + String(humedad));
            float tempDS18B20 = line.substring(line.lastIndexOf(',', secondLastComma - 1) + 1, secondLastComma).toFloat();

             // Para debug
            Serial.println("Valores extraídos:");
            Serial.println("tempDHT: " + String(tempDHT));
            Serial.println("humedad: " + String(humedad));
            Serial.println("tempDS18B20: " + String(tempDS18B20));
            // Enviar datos
            publishData(datePart, timePart, tempDHT, humedad, tempDS18B20);
            
            // Actualizar la línea con sent=1
            line = line.substring(0, lastComma + 1) + "1";
            dataModified = true;
            Serial.println("Datos enviados y línea actualizada: " + line);
            
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        
        updatedLines.push_back(line);
    }
    file.close();

    // Si hubo modificaciones, actualizar el archivo
    if (dataModified) {
        SPIFFS.remove("/deviceDataSensor.csv");
        File outFile = SPIFFS.open("/deviceDataSensor.csv", FILE_WRITE);
        
        if (outFile) {
            for (const String& updatedLine : updatedLines) {
                outFile.println(updatedLine);
            }
            outFile.close();
            Serial.println("Archivo CSV actualizado con nuevos estados de envío");
            
            // Mostrar contenido actualizado
            File readFile = SPIFFS.open("/deviceDataSensor.csv", FILE_READ);
            if (readFile) {
                Serial.println("Contenido actual del archivo CSV:");
                while (readFile.available()) {
                    String line = readFile.readStringUntil('\n');
                    Serial.println(line);
                }
                readFile.close();
            }
        }
    }
}
 */
/* void sendStoredData() {

    // 1. Verificaciones previas
    if (!isWiFiConnected()) {
        Serial.println("Sin conexión WiFi - No se pueden enviar datos almacenados");
        return;
    }

    if (!isMQTTConnected()) {
        Serial.println("Sin conexión MQTT - No se pueden enviar datos almacenados");
        return;
    }

    std::vector<String> fileLines;
    std::vector<String> pendingLines;
    bool allDataSent = true;

    // 2. Leer TODO el archivo dentro del mutex
    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY) != pdTRUE) {
        Serial.println("No se pudo tomar mutex SPIFFS");
        return;
    }

    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_READ);
    if (!file) {
        Serial.println("No hay datos almacenados para enviar.");
        xSemaphoreGive(spiffsMutex);
        return;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
            fileLines.push_back(line);
        }
    }

    file.close();
    xSemaphoreGive(spiffsMutex);

    // 3. Procesar y enviar datos (SIN mutex)
    for (const String& line : fileLines) {

        std::vector<String> values;
        int start = 0; 
        int end = line.indexOf(',');

        while (end != -1) {
            values.push_back(line.substring(start, end));
            start = end + 1;
            end = line.indexOf(',', start);
        }
        values.push_back(line.substring(start));

        if (values.size() != 6) {
            Serial.println("Línea con formato incorrecto: " + line);
            continue;
        }

        int sent = values[5].toInt();

        if (sent == 0 && isMQTTConnected()) {

            String datePart = values[0];
            String timePart = values[1];
            float tempDHT = values[2].toFloat();
            float humedad = values[3].toFloat();
            float tempDS18B20 = values[4].toFloat();

            if (publishData(datePart, timePart, tempDHT, humedad, tempDS18B20)) {
                Serial.println("Datos enviados correctamente: " + line);
            } else {
                Serial.println("Error al enviar datos, se conservarán.");
                pendingLines.push_back(line);
                allDataSent = false;
            }

        } else {
            pendingLines.push_back(line);
            allDataSent = false;
        }
    }

    // 4. Escribir nuevamente el archivo (solo lo pendiente)
    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY) != pdTRUE) {
        Serial.println("No se pudo tomar mutex SPIFFS para escritura");
        return;
    }

    File outFile = SPIFFS.open("/deviceDataSensor.csv", FILE_WRITE);
    if (!outFile) {
        Serial.println("Error abriendo archivo para escritura");
        xSemaphoreGive(spiffsMutex);
        return;
    }

    if (!allDataSent) {
        for (const String& pendingLine : pendingLines) {
            outFile.println(pendingLine);
        }
        Serial.println("Archivo actualizado con datos pendientes.");
    } else {
        Serial.println("Todos los datos fueron enviados, archivo limpiado.");
    }

    outFile.close();
    xSemaphoreGive(spiffsMutex);
}

 */

 /* void sendStoredData() {

    if (!isWiFiConnected() || !isMQTTConnected()) {
        Serial.println("Sin conexión - Envío pospuesto");
        return;
    }

    std::vector<String> fileLines;
    std::vector<String> pendingLines;

    // 1️⃣ Leer archivo original
    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY ) != pdTRUE) {
        Serial.println("No se pudo tomar mutex SPIFFS");
        return;
    }

    if (!SPIFFS.exists("/deviceDataSensor.csv")) {
        xSemaphoreGive(spiffsMutex);
        Serial.println("No hay archivo CSV");
        return;
    }

    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_READ);
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
            fileLines.push_back(line);
        }
    }
    file.close();
    xSemaphoreGive(spiffsMutex);

    // 2️⃣ Procesar y enviar (sin mutex)
    for (const String& line : fileLines) {

        std::vector<String> values;
        int start = 0;
        int end = line.indexOf(',');

        while (end != -1) {
            values.push_back(line.substring(start, end));
            start = end + 1;
            end = line.indexOf(',', start);
        }
        values.push_back(line.substring(start));

        // Línea corrupta → se descarta
        if (values.size() != 6) {
            Serial.println("Línea corrupta descartada: " + line);
            continue;
        }

        float tempDHT = values[2].toFloat();
        float humedad = values[3].toFloat();
        float tempDS18 = values[4].toFloat();

        if (!publishData(values[0], values[1], tempDHT, humedad, tempDS18)) {
            pendingLines.push_back(line);
        }
    }

    // 3️⃣ Escritura segura en archivo temporal
    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY) != pdTRUE) {
        Serial.println("No se pudo tomar mutex para escritura");
        return;
    }

    File tmpFile = SPIFFS.open("/deviceDataSensor.tmp", FILE_WRITE);
    if (!tmpFile) {
        xSemaphoreGive(spiffsMutex);
        Serial.println("Error creando archivo temporal");
        return;
    }

    for (const String& pending : pendingLines) {
        tmpFile.println(pending);
    }

    tmpFile.close();

    // 4️⃣ Commit atómico
    SPIFFS.remove("/deviceDataSensor.csv");
    SPIFFS.rename("/deviceDataSensor.tmp", "/deviceDataSensor.csv");

    xSemaphoreGive(spiffsMutex);

    Serial.println("Envío completado y estado guardado de forma segura");
} */
bool sendStoredData() {

    if (!isWiFiConnected() || !isMQTTConnected()) {
        return false;
    }

    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY) != pdTRUE) {
        return false;
    }

    if (!LittleFS.exists("/deviceDataSensor.csv")) {
        xSemaphoreGive(spiffsMutex);
        return false;
    }

    File file = LittleFS.open("/deviceDataSensor.csv", FILE_READ);
    if (!file) {
        xSemaphoreGive(spiffsMutex);
        return false;
    }

    lastConfirmedSequence = nvs.getUInt("lastConfirmedSeq", 0);
    uint32_t maxSequenceInFile = 0;
    bool sentSomething = false;

    while (file.available()) {

        String line = file.readStringUntil('\n');
        line.trim();
        if (line.isEmpty()) continue;

        char buffer[128];
        if (line.length() >= sizeof(buffer)) continue;
        line.toCharArray(buffer, sizeof(buffer));

        char *token = strtok(buffer, ","); if (!token) continue;
        String date = token;

        token = strtok(NULL, ","); if (!token) continue;
        String time = token;

        token = strtok(NULL, ","); if (!token) continue;
        float tempDHT = atof(token);

        token = strtok(NULL, ","); if (!token) continue;
        float humedad = atof(token);

        token = strtok(NULL, ","); if (!token) continue;
        float tempDS18 = atof(token);

        token = strtok(NULL, ","); if (!token) continue;
        uint32_t sequence = atoi(token);

        Serial.println("esta es la secuancia en el almacenamiento : ");
        Serial.println(sequence);

        maxSequenceInFile = max(maxSequenceInFile, sequence);

        if (sequence <= lastConfirmedSequence) {
            continue;
        }

        if (!publishStorageData(date, time, tempDHT, humedad, tempDS18, sequence)) {
            break;
        }

        lastConfirmedSequence = sequence;
        nvs.putUInt("lastConfirmedSeq", lastConfirmedSequence);
        sentSomething = true;
    }

    file.close();

    // 🧹 Eliminación SEGURA
    if (maxSequenceInFile > 0 && maxSequenceInFile <= lastConfirmedSequence) {
        Serial.println("Todos los datos confirmados. Eliminando archivo.");
        LittleFS.remove("/deviceDataSensor.csv");
    }

    xSemaphoreGive(spiffsMutex);
    return sentSomething;
}



void recoverSPIFFSState() {

    if (xSemaphoreTake(spiffsMutex, portMAX_DELAY) != pdTRUE) {
        Serial.println("No se pudo tomar mutex SPIFFS para recuperación");
        return;
    }

    if (LittleFS.exists("/deviceDataSensor.tmp")) {
        Serial.println("Archivo temporal detectado. Recuperando estado...");

        LittleFS.remove("/deviceDataSensor.csv");
        LittleFS.rename("/deviceDataSensor.tmp", "/deviceDataSensor.csv");

        Serial.println("Recuperación completada.");
    }

    xSemaphoreGive(spiffsMutex);
}




