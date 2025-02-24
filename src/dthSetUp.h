#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
bool dthSensorsetUp (SemaphoreHandle_t lcdSemaphore);
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad);
float dhtGetTemperature();
float dhtGetHumidity();
    