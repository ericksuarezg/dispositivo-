#include <SPIFFS.h>
#include <WiFi.h>
#include <mqtt.h>
#include <LcdSetUp.h>

void setupSPIFFS() {
    delay(1000);
    if (!SPIFFS.begin(true)) {
        Serial.println("Error inicializando SPIFFS");
        return;
    }
    Serial.println("SPIFFS Confivgurado exitosamente.");
    

}


void saveDataToCSV(float tempDHT, float humedad, float tempDS18B20, unsigned long timestamp) {
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
                      String(tempDS18B20) /* + "," + String(timestamp) */;
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
        while (file.available()) {
            String line = file.readStringUntil('\n');
            //Serial.println("Leyendo línea: " + line);
    
            // Dividir la línea en valores
            int comma1 = line.indexOf(',');
            int comma2 = line.indexOf(',', comma1 + 1);
            int comma3 = line.indexOf(',', comma2 + 1);
    
            float tempDHT = line.substring(0, comma1).toFloat();
            float humedad = line.substring(comma1 + 1, comma2).toFloat();
            float tempDS18B20 = line.substring(comma2 + 1, comma3).toFloat();
            unsigned long timestamp = line.substring(comma3 + 1).toInt();
            
            Serial.println(tempDHT);
            Serial.println(humedad);
            Serial.println(tempDS18B20);
            // Publicar los datos utilizando la función MQTT
            storagePublishData(tempDHT, humedad, tempDS18B20);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        file.close();
    
        // Borrar archivo tras enviar los datos
        //SPIFFS.remove("/sensorData.csv");
        Serial.println("Datos almacenados enviados y archivo CSV eliminado.");
        xSemaphoreGive(lcdSemaphore);
    }
    
}