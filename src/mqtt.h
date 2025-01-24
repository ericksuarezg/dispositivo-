#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
void mqttSetUp(SemaphoreHandle_t lcdSemaphore);
void reconnect(SemaphoreHandle_t lcdSemaphore);
void CheckForMessages();
void publishData(String date, String time, float temperaturaDHT,float humedadRelativa, float temperaturaDS18);
bool isMQTTConnected();
void storagePublishData(String datePart, String timePart, float temperaturaDHT,float humedadRelativa, float temperaturaDS18); 
void publishAlerts(String datePart, String timePart, String rangeTipe, String varName, float alertVatiable);
void publishAlertsSensorOutService(String datePart, String timePart, String messageSensorOutService, String sensorName);