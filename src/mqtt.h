#include <freertos/FreeRTOS.h> 
#include <freertos/semphr.h> 
extern SemaphoreHandle_t sequenceMutex;
void mqttSetUp(SemaphoreHandle_t lcdSemaphore,TaskHandle_t TaskSendHandle);
void reconnect(SemaphoreHandle_t lcdSemaphore, TaskHandle_t TaskSendHandle);
void CheckForMessages();
bool publishData(String date, String time, float temperaturaDHT,float humedadRelativa, float temperaturaDS18);
bool isMQTTConnected();
void storagePublishData(String datePart, String timePart, float temperaturaDHT,float humedadRelativa, float temperaturaDS18); 
void publishAlerts(String datePart, String timePart, String rangeTipe, String varName, float alertVatiable);
void publishAlertsSensorOutService(String datePart, String timePart, String messageSensorOutService, String sensorName);
bool publishStorageData(String date, String time, float temperaturaDHT, float humedadRelativa, float temperaturaDS18, uint32_t secuence);