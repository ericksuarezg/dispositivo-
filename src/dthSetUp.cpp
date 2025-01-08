#include <DHT.h> 
#include <LcdSetUp.h>
#define DHT_PIN 32
DHT dht(DHT_PIN, DHT22); 
void dthSensorsetUp (){
   displayInfoOnLCD("  Configurando","     DHT22");
   vTaskDelay(3000/ portTICK_PERIOD_MS);
   dht.begin();
   float humidity = dht.readHumidity();
   float temperature = dht.readTemperature();
 
   if (isnan(humidity) || isnan(temperature)) {
     // Error al leer el sensor
     displayInfoOnLCD("Error:", "Sensor DHT22");
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
   }
   vTaskDelay(5000 / portTICK_PERIOD_MS);
}
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad) {
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