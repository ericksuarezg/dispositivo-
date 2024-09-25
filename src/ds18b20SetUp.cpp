#include <DallasTemperature.h>
#include <OneWire.h>
#include <LcdSetUp.h>

const int oneWireBus=33;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);



void ds18b20SetUp(SemaphoreHandle_t lcdSemaphore){
  sensors.begin();
  displayInfoOnLCD("Setting up","ds18b20");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
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