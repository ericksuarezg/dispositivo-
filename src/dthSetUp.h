#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
bool dthSensorsetUp ();
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad);