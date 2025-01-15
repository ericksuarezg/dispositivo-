#include <SPIFFS.h>
#include <WiFi.h>
//#include <mqtt.h>
#include <mqttSecurity.h>
#include <LcdSetUp.h>
#include <vector>


void setupSPIFFS() {
    delay(1000);
    if (!SPIFFS.begin(true)) {
        Serial.println("Error inicializando SPIFFS");
        return;
    }
    SPIFFS.remove("/sensorData.csv");
    Serial.println("SPIFFS Confivgurado exitosamente.");
    

}


void saveDataToCSV(float tempDHT, float humedad, float tempDS18B20, unsigned long timestamp,int toSend) {
    if (isnan(tempDHT) || isnan(humedad) || isnan(tempDS18B20) || tempDS18B20==-127) {
    Serial.println("Error: Datos inválidos. No se almacenara información.");
    return; 
  }
    File file = SPIFFS.open("/sensorData.csv", FILE_APPEND);
    if (!file) {
        Serial.println("Error abriendo archivo CSV.");
        return;
    }
    String dataLine = String(tempDHT) + "," + String(humedad) + "," +
                      String(tempDS18B20) /* + "," + String(timestamp) */+ ","+
                      String(toSend) + ",0";
    file.println(dataLine);
    file.close();
    Serial.println("Datos guardados en CSV: " + dataLine);
}

void sendStoredData(SemaphoreHandle_t lcdSemaphore) {
    if (xSemaphoreTake(lcdSemaphore,5000/portMAX_DELAY)==pdTRUE){
         File file = SPIFFS.open("/sensorData.csv", FILE_READ);
        if (!file) {
            Serial.println("No hay datos almacenados para enviar.");
            xSemaphoreGive(lcdSemaphore);
            return;
        }
        std::vector<String> updatedLines;
        while (file.available()) {
            String line = file.readStringUntil('\n');
            //Serial.println("Leyendo línea: " + line);
    
            // Dividir la línea en valores
            int comma1 = line.indexOf(',');
            int comma2 = line.indexOf(',', comma1 + 1);
            int comma3 = line.indexOf(',', comma2 + 1);
            int comma4 = line.indexOf(',', comma3 + 1);
            int comma5 = line.indexOf(',', comma4 + 1);
    
            float tempDHT = line.substring(0, comma1).toFloat();
            float humedad = line.substring(comma1 + 1, comma2).toFloat();
            float tempDS18B20 = line.substring(comma2 + 1, comma3).toFloat();
            unsigned long timestamp = line.substring(comma3 + 1,comma4).toInt();
            int toSend = line.substring(comma4 + 1, comma5).toInt();
            int sent = line.substring(comma5 + 1).toInt();
            Serial.println(tempDHT);
            Serial.println(humedad);
            Serial.println(tempDS18B20);
            Serial.print("to Send: ");
            Serial.print(toSend);
            Serial.println("sent: ");
            Serial.print(sent);

            // Publicar los datos utilizando la función MQTT
            if (toSend == 1 && sent == 0) {
                // Publicar los datos utilizando la función MQTT
                storagePublishData(tempDHT, humedad, tempDS18B20);

                // Actualizar el estado del registro a enviado
                line = String(tempDHT) + "," + String(humedad) + "," +
                       String(tempDS18B20) + "," /* + String(timestamp) */ + ",1,1";
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            // Agregar la línea al vector actualizado
            updatedLines.push_back(line);
            //storagePublishData(tempDHT, humedad, tempDS18B20);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        file.close();
        // Reescribir el archivo con los datos actualizados
        File outFile = SPIFFS.open("/sensorData.csv", FILE_WRITE);
        if (outFile) {
            for (const String& updatedLine : updatedLines) {
                outFile.println(updatedLine);
            }
            outFile.close();
        }
    
        // Borrar archivo tras enviar los datos
        //SPIFFS.remove("/sensorData.csv");
        Serial.println("Datos almacenados enviados y archivo CSV eliminado.");
        xSemaphoreGive(lcdSemaphore);
    }
    
}