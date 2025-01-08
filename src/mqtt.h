#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
void mqttSetUp(SemaphoreHandle_t lcdSemaphore);
void reconnect(SemaphoreHandle_t lcdSemaphore);
void CheckForMessages();
void publishData(SemaphoreHandle_t lcdSemaphore, float temperaturaDHT,float humedadRelativa, float temperaturaDS18);
bool isMQTTConnected();