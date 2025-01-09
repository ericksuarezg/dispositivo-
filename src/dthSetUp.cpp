#include <DHT.h> 
#include <LcdSetUp.h>
#define DHT_PIN 32

static bool dhtConfigured = false;
DHT dht(DHT_PIN, DHT22); 

bool dthSensorsetUp (){
   displayInfoOnLCD("  Configurando","     DHT22");
   vTaskDelay(3000/ portTICK_PERIOD_MS);
   dht.begin();
   float humidity = dht.readHumidity();
   float temperature = dht.readTemperature();
 
   if (isnan(humidity) || isnan(temperature)) {
     // Error al leer el sensor
     displayInfoOnLCD("Error:", "Sensor DHT22");
     Serial.println("Error: No se pudo configurar el sensor DHT22.");
     dhtConfigured = false;
     vTaskDelay(5000 / portTICK_PERIOD_MS);
     return false;
   } else {
     // Sensor funcionando correctamente, muestra los datos
     displayInfoOnLCD("Sensor DTH","OK");
     vTaskDelay(2000/ portTICK_PERIOD_MS);
     displayDataOnLCDofDHT(temperature,humidity);
     vTaskDelay(2000/ portTICK_PERIOD_MS);
     //calculamos la sensacion termica
     float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
     char termicSen[16];
     snprintf(termicSen, sizeof(termicSen), "%.1f C", heatIndex);
     displayInfoOnLCD("Sens. termica de",termicSen);
     dhtConfigured = true;
     vTaskDelay(5000 / portTICK_PERIOD_MS);
     return true;
   }
  
}
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad) {
    if (!dhtConfigured) {
      Serial.println("Sensor DHT22 no configurado. Intentando configurar nuevamente.");
      if (xSemaphoreTake(lcdSemaphore,3000 / portMAX_DELAY)==pdTRUE){
        dhtConfigured = dthSensorsetUp();
        xSemaphoreGive(lcdSemaphore);
      }
      if (!dhtConfigured)
      {
        Serial.println("Error: No se pudo configurar el sensor DHT22.");
        temperaturaDHT = NAN;
        humedad = NAN; // Valores no válidos para indicar error
        return;
      }else{
        if (xSemaphoreTake(lcdSemaphore,3000 / portMAX_DELAY)==pdTRUE)
        {
         displayInfoOnLCD(" Config exitosa", "de sensor DTH22");
         vTaskDelay(3000 / portTICK_PERIOD_MS);
         xSemaphoreGive(lcdSemaphore);
         return;
        }
      }
    }  
    temperaturaDHT = dht.readTemperature();
    humedad = dht.readHumidity();
    Serial.print(temperaturaDHT);
    Serial.print(humedad);
    
    if (isnan(temperaturaDHT) || isnan(humedad)) {
      if (xSemaphoreTake(lcdSemaphore,3000 / portMAX_DELAY)==pdTRUE)
      {
        Serial.println("Error al leer el sensor DHT22");
        displayInfoOnLCD("error DTH", "Not reading it");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
      }
      
    }else
    {
      if (xSemaphoreTake(lcdSemaphore,3000 / portMAX_DELAY)==pdTRUE)
      {
        displayDataOnLCDofDHT(temperaturaDHT,humedad);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
      }
    }
}