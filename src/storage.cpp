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
    //SPIFFS.remove("/deviceDataSensor.csv");
    Serial.println("SPIFFS Confivgurado exitosamente.");
    

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
    float latitude = jsonDoc["lat"].as<float>();  // Latitud es normalmente un valor decimal
    float longitude = jsonDoc["lon"].as<float>(); // Longitud es normalmente un valor decimal
    String timeZone = jsonDoc["timezone"] | "N/A"; // Corrige el nombre y el tipo de datos
    String isp = jsonDoc["isp"] | "N/A";
    String org = jsonDoc["org"] | "N/A";
    String as = jsonDoc["as"] | "N/A";

    // Abrir el arcivo CSV
    File file = SPIFFS.open("/deviceDataSensor.csv", FILE_APPEND);
    if (!file) {
        Serial.println("Error abriendo archivo CSV.");    
        return;
    }

    // Crear línea de datos para el CSV
    String dataLine =   country + "," + regionName + "," + city + "," + zipCode + "," + latitude + "," + longitude + "," + 
                        timeZone + "," + isp + "," + org + "," + as + "," + datePart + "," + timePart + "," + 
                        String(tempDHT) + "," + String(humedad) + "," + String(tempDS18B20) + "," + String(toSend) + "," + "0";
    file.println(dataLine);
    file.close();
    Serial.println("Datos guardados en CSV: " + dataLine);
}

void sendStoredData() {
    
        File file = SPIFFS.open("/deviceDataSensor.csv", FILE_READ);
        if (!file) {
            Serial.println("No hay datos almacenados para enviar.");
            return;
        }
        
        std::vector<String> updatedLines;

        // Leer el archivo linea por linea
        while (file.available()) {
            String line = file.readStringUntil('\n');
            //Serial.println("Leyendo línea: " + line);

            // Contar cuántas comas hay en la línea, asegurando que el archivo esté bien formado
            int commaCount = std::count(line.begin(), line.end(), ',');
            if (commaCount != 16) {  // Cambiar 14 a la cantidad de comas que esperas
                continue;  // Saltamos esta línea si no es válida
            }

            // Dividir la línea en valores
            int comma1 = line.indexOf(',');
            int comma2 = line.indexOf(',', comma1 + 1);
            int comma3 = line.indexOf(',', comma2 + 1);
            int comma4 = line.indexOf(',', comma3 + 1);
            int comma5 = line.indexOf(',', comma4 + 1);
            int comma6 = line.indexOf(',', comma5 + 1);
            int comma7 = line.indexOf(',', comma6 + 1);
            int comma8 = line.indexOf(',', comma7 + 1);
            int comma9 = line.indexOf(',', comma8 + 1);
            int comma10 = line.indexOf(',', comma9 + 1);
            int comma11 = line.indexOf(',', comma10 + 1);
            int comma12 = line.indexOf(',', comma11 + 1);
            int comma13 = line.indexOf(',', comma12 + 1);
            int comma14 = line.indexOf(',', comma13 + 1);
            int comma15 = line.indexOf(',', comma14 + 1);
            int comma16 = line.indexOf(',', comma15 + 1);
            int comma17 = line.indexOf(',', comma16 + 1);

                  
            // Extraer valores de cada parte de la línea CSV
            String country = line.substring(0, comma1);
            String regionName = line.substring(comma1 + 1, comma2);
            String city = line.substring(comma2 + 1, comma3);
            int zipCode = line.substring(comma3 + 1, comma4).toInt();
            float latitude = line.substring(comma4 + 1, comma5).toFloat();
            float longitude = line.substring(comma5 + 1, comma6).toFloat();
            String timeZone = line.substring(comma6 + 1, comma7);
            String isp = line.substring(comma7 + 1, comma8);
            String org = line.substring(comma8 + 1, comma9);
            String as = line.substring(comma9 + 1, comma10);
            String datePart = line.substring(comma10 + 1, comma11);
            String timePart = line.substring(comma11 + 1, comma12);
            float tempDHT = line.substring(comma12 + 1, comma13).toFloat();
            float humedad = line.substring(comma13 + 1, comma14).toFloat();
            float tempDS18B20 = line.substring(comma14 + 1, comma15).toFloat();
            int toSend = line.substring(comma15 + 1, comma16).toInt();
            int sent = line.substring(comma16 + 1).toInt();

            
            // Validar cada campo extraído
            if (country.length() == 0) country = "N/A";
            if (regionName.length() == 0) regionName = "N/A";
            if (city.length() == 0) city = "N/A";
            if (timeZone.length() == 0) timeZone = "N/A";
            if (isp.length() == 0) isp = "N/A";
            if (org.length() == 0) org = "N/A";
            if (as.length() == 0) as = "N/A";
            if (datePart.length() == 0) datePart = "N/A";
            if (timePart.length() == 0) timePart = "N/A";
            if (zipCode == 0) zipCode = -1;
            if (isnan(latitude)) latitude = 0.0;
            if (isnan(longitude)) longitude = 0.0;
            if (isnan(tempDHT)) tempDHT = 0.0;
            if (isnan(humedad)) humedad = 0.0;
            if (isnan(tempDS18B20)) tempDS18B20 = 0.0;

            // Publicar los datos utilizando la función MQTT
            if (toSend == 1 && sent == 0) {
                // Publicar los datos utilizando la función MQTT
                storagePublishData(datePart, timePart, tempDHT, humedad, tempDS18B20);

                // Actualizar el estado del registro a enviado
                line = datePart + "," + timePart + "," + 
                        String(tempDHT) + "," + String(humedad) + "," + String(tempDS18B20) + "," + String(toSend) + "," + "1";
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            // Agregar la línea al vector actualizado
            updatedLines.push_back(line);
            //storagePublishData(datePart, timePart, tempDHT, humedad, tempDS18B20);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            
        }
        file.close();
        // Reescribir el archivo con los datos actualizados
        File outFile = SPIFFS.open("/deviceDataSensor.csv", FILE_WRITE);
        if (outFile) {
            for (const String& updatedLine : updatedLines) {
                outFile.println(updatedLine);
            }
            outFile.close();
        }

        // Borrar archivo tras enviar los datos
        //SPIFFS.remove("/sensorData.csv");
        Serial.println("Datos almacenados enviados y archivo CSV eliminado.");
    
}







