#include <DallasTemperature.h>
#include <OneWire.h>
#include <LcdSetUp.h>

const int oneWireBus=33; 
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);



void ds18b20SetUp(SemaphoreHandle_t lcdSemaphore){
  sensors.begin();
  displayInfoOnLCD("   Configurando","    ds18b20");
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  // Verifica si hay sensores conectados
  if (sensors.getDeviceCount() == 0) {
    // Si no hay sensores, muestra un mensaje de error en el LCD
    displayInfoOnLCD(     "Error:", "No DS18B20 found");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  } else {
    // Si el sensor está conectado, muestra un mensaje de configuración
    displayInfoOnLCD(" Sensor DS18B20", "OK");
    vTaskDelay(5000 / portTICK_PERIOD_MS);  // Espera 5 segundos
  }
  xSemaphoreGive(lcdSemaphore);
}

void ds18b20ReadTemperature(SemaphoreHandle_t lcdSemaphore,float &temperatureCDs18b20){
  sensors.requestTemperatures();
  temperatureCDs18b20 = sensors.getTempCByIndex(0);
  xSemaphoreTake(lcdSemaphore,portMAX_DELAY);
  displayDataOnLCDofDbs18b20(temperatureCDs18b20);
  Serial.print(temperatureCDs18b20);
  Serial.println("ºC");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xSemaphoreGive(lcdSemaphore);
}