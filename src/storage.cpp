#include <SPIFFS.h>
#include <WiFi.h>
#include <mqtt.h>
#include <LcdSetUp.h>
#include <ArduinoJson.h>
#include <vector>


void setupSPIFFS() {
    delay(1000);
    if (!SPIFFS.begin(true)) {
        Serial.println("Error inicializando SPIFFS");
        return;
    }
    Serial.println("SPIFFS Configurado exitosamente.");
}

void saveDataToCSV(String payload, String datePart, String timePart, float tempDHT, float humedad, float tempDS18B20, int toSend) {
    
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

void readAndUpdateCSV() {
    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_READ);
    if (!file) {
        Serial.println("No se puede abrir el archivo para lectura");
        return;
    }

    std::vector<String> lines;
    bool needsUpdate = false;

    // Leer todas las líneas del archivo
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        
        if (line.length() > 0) {
            // Verificar si la línea tiene datos para enviar y no han sido enviados
            int lastComma = line.lastIndexOf(',');
            int secondLastComma = line.lastIndexOf(',', lastComma - 1);
            
            int sent = line.substring(lastComma + 1).toInt();
            int toSend = line.substring(secondLastComma + 1, lastComma).toInt();

            if (toSend == 1 && sent == 0 && isMQTTConnected()) {
                // Extraer los datos necesarios
                int startDataIndex = line.lastIndexOf(',', secondLastComma - 1);
                for (int i = 0; i < 3; i++) {
                    startDataIndex = line.lastIndexOf(',', startDataIndex - 1);
                }
                
                String datePart = line.substring(line.lastIndexOf(',', startDataIndex - 1) + 1, 
                                               line.indexOf(',', line.lastIndexOf(',', startDataIndex - 1) + 1));
                String timePart = line.substring(line.lastIndexOf(',', startDataIndex) + 1, 
                                               line.indexOf(',', line.lastIndexOf(',', startDataIndex) + 1));
                float tempDHT = line.substring(startDataIndex + 1, 
                                             line.indexOf(',', startDataIndex + 1)).toFloat();
                float humedad = line.substring(line.indexOf(',', startDataIndex + 1) + 1, 
                                            line.indexOf(',', line.indexOf(',', startDataIndex + 1) + 1)).toFloat();
                float tempDS18B20 = line.substring(line.lastIndexOf(',', secondLastComma - 1) + 1, 
                                                 secondLastComma).toFloat();

                // Publicar los datos usando publishData
                //publishData(NULL, datePart, timePart, tempDHT, humedad, tempDS18B20);
                
                // Marcar como enviado
                line = line.substring(0, lastComma + 1) + "1";
                needsUpdate = true;
            }
            lines.push_back(line);
        }
    }
    file.close();

    // Si se realizaron cambios, actualizar el archivo
    if (needsUpdate) {
        File writeFile = SPIFFS.open("/deviceDataSensor.csv", FILE_WRITE);
        if (!writeFile) {
            Serial.println("Error al abrir el archivo para escritura");
            return;
        }

        for (const String& line : lines) {
            writeFile.println(line);
        }
        writeFile.close();
        Serial.println("Archivo CSV actualizado exitosamente");
    }
}



/*
void sendStoredData() {
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

        Serial.println("Estado actual - toSend: " + String(toSend) + ", sent: " + String(sent));

        if (toSend == 1 && sent == 0 && isMQTTConnected()) {
            // Extraer los datos necesarios para enviar
            int startDataIndex = line.lastIndexOf(',', secondLastComma - 1);
            for (int i = 0; i < 3; i++) {
                startDataIndex = line.lastIndexOf(',', startDataIndex - 1);
            }
            
            String datePart = line.substring(line.lastIndexOf(',', startDataIndex - 1) + 1, line.indexOf(',', line.lastIndexOf(',', startDataIndex - 1) + 1));
            String timePart = line.substring(line.lastIndexOf(',', startDataIndex) + 1, line.indexOf(',', line.lastIndexOf(',', startDataIndex) + 1));
            float tempDHT = line.substring(startDataIndex + 1, line.indexOf(',', startDataIndex + 1)).toFloat();
            float humedad = line.substring(line.indexOf(',', startDataIndex + 1) + 1, line.indexOf(',', line.indexOf(',', startDataIndex + 1) + 1)).toFloat();
            float tempDS18B20 = line.substring(line.lastIndexOf(',', secondLastComma - 1) + 1, secondLastComma).toFloat();

            // Enviar datos
            //storagePublishData(datePart, timePart, tempDHT, humedad, tempDS18B20);
            
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







