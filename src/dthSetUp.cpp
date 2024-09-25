#include <DHT.h> 
#include <LcdSetUp.h>
#define DHT_PIN 32
DHT dht(DHT_PIN, DHT22);
void dthSensorsetUp (){
    dht.begin();
    displayInfoOnLCD("config sensor", "DHT");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad) {
    temperaturaDHT = dht.readTemperature();
    humedad = dht.readHumidity();
    Serial.print(temperaturaDHT);
    Serial.print(humedad);
    
    if (isnan(temperaturaDHT) || isnan(humedad)) {
      Serial.println("Error al leer el sensor DHT22");
      xSemaphoreTake(lcdSemaphore,portMAX_DELAY);
      displayInfoOnLCD("error DTH", "Not reading it");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
    }else
    {
      xSemaphoreTake(lcdSemaphore,portMAX_DELAY);
      displayDataOnLCDofDHT(temperaturaDHT,humedad);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
    }
}