#include <DallasTemperature.h>
#include <OneWire.h>
#include <LcdSetUp.h>

const int oneWireBus=33; 
static bool ds18b20Configured = false;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);




bool ds18b20SetUp(SemaphoreHandle_t lcdSemaphore){
  sensors.begin();
  displayInfoOnLCD("   Configurando","    ds18b20");
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  // Verifica si hay sensores conectados
  if (sensors.getDeviceCount() == 0) {
    // Si no hay sensores, muestra un mensaje de error en el LCD
    displayInfoOnLCD(     "Error:", "No DS18B20 found");
    Serial.println("Error: No se detectaron sensores DS18B20.");
    ds18b20Configured = false;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xSemaphoreGive(lcdSemaphore);
    return ds18b20Configured;
  } else {
    // Si el sensor está conectado, muestra un mensaje de configuración
    displayInfoOnLCD(" Sensor DS18B20", "OK");
    Serial.println("Sensor DS18B20 configurado correctamente.");
    ds18b20Configured = true;
    vTaskDelay(5000 / portTICK_PERIOD_MS);  // Espera 5 segundos
    xSemaphoreGive(lcdSemaphore);
    return ds18b20Configured;
  }
}

void ds18b20ReadTemperature(SemaphoreHandle_t lcdSemaphore,float &temperatureCDs18b20){
  if (!ds18b20Configured || (temperatureCDs18b20== -127)) {
    Serial.println("Sensor DS18B20 no configurado. Saltando lectura.");
    if (xSemaphoreTake(lcdSemaphore,3000 / portMAX_DELAY)==pdTRUE){
      displayInfoOnLCD(" Sensor DS18B20", "No configurado");
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      ds18b20Configured = ds18b20SetUp(lcdSemaphore);
      if (!ds18b20Configured) {
        temperatureCDs18b20 = NAN; // Valor no válido
        return;
      }else{
        return;
      }
    }
    temperatureCDs18b20 = NAN; // Valor no válido
    return;
  }
  sensors.requestTemperatures();
  temperatureCDs18b20 = sensors.getTempCByIndex(0);
  if (xSemaphoreTake(lcdSemaphore,5000 / portMAX_DELAY)==pdTRUE){
    displayDataOnLCDofDbs18b20(temperatureCDs18b20);
    Serial.print(temperatureCDs18b20);
    Serial.println("ºC");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xSemaphoreGive(lcdSemaphore);
  } 
}