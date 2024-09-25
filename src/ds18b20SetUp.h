#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
void ds18b20SetUp(SemaphoreHandle_t lcdSemaphore);
void ds18b20ReadTemperature(SemaphoreHandle_t lcdSemaphore,float &temperatureCDs18b20);

