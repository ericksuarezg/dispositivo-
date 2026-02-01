#include <Arduino.h>
#include <WiFiManagerSetUp.h>
#include <LcdSetUp.h>
#include <mqtt.h>
#include <dthSetUp.h>
#include <ds18b20SetUp.h>
#include <timeSetUp.h>
#include "freertos/semphr.h" 
#include <storage.h>
#include <alert.h>
#include "PublishingInit.h"


// Definir los "handle" de las tareas
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;

TaskHandle_t TaskSendHandle = NULL;


// Declarar el semáforo para WiFi
SemaphoreHandle_t wifiSemaphore;
// declarar semaforo para lcd 
SemaphoreHandle_t lcdSemaphore;
//semaforo para controlar el acceso a SPIFF 
SemaphoreHandle_t spiffsMutex;

// Función para la primera tarea
void conectToInternet(void *pvParameters) {
    setUpWifi(wifiSemaphore,lcdSemaphore);
    mqttSetUp(lcdSemaphore, TaskSendHandle);
    //xSemaphoreGive(wifiSemaphore);
    while (true) {
        Serial.println("verificando conexion a Wifi y Mqtt en ejecucion");
        reconectWiFi(lcdSemaphore);
        localTimeSetUp(); 
        reconnect(lcdSemaphore, TaskSendHandle); 
        CheckForMessages();
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Espera de 1 segundo
    }
}


// Función para la segunda tarea
void Task2(void *pvParameters) { 
    float temperaturaDHT;
    float humedad;
    float temperatureCDs18b20;
    float tempDHTMin = -25;
    float tempDHTMax = 85;
    float humidityMin = 10;
    float humidityMax = 110;
    float tempDS18Min = -25;
    float tempDS18Max = 85;

    setupSPIFFS();
    recoverSPIFFSState();
    setUpLcd(wifiSemaphore);
    dthSensorsetUp();
    ds18b20SetUp(lcdSemaphore);
    configurarAlertas(tempDHTMin, tempDHTMax, humidityMin, humidityMax, tempDS18Min, tempDS18Max);
    
    TickType_t lastWakeTime = xTaskGetTickCount();
    TickType_t lastSaveTime = lastWakeTime;
    TickType_t lastPublishTime = lastWakeTime;
    const TickType_t publishInterval = 10800000; // 1 horas en milisegundos
    //const TickType_t publishInterval = 5000; // 1 horas en milisegundos
    const TickType_t saveInterval = 180000; // 3 minutos en milisegundos
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    while (true) {
        Serial.println("ejecutando lectura de sensores");
        ds18b20ReadTemperature(lcdSemaphore,temperatureCDs18b20);
        dhtReading(lcdSemaphore,temperaturaDHT,humedad);
        updateClockDisplay(lcdSemaphore);
          // Almacenar datos periódicamente
        // Verificar intervalo de guardado  
        if (xTaskGetTickCount() - lastSaveTime >= saveInterval) {
            // Verificar Alertas
            
            //verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
            lastSaveTime = xTaskGetTickCount();
        }
        
        if (xTaskGetTickCount() - lastPublishTime >= publishInterval) {
            // Verificar alertas
            //verificarAlertas(temperaturaDHT, humedad, temperatureCDs18b20);
        
            // Verificar conexión antes de publicar
            if (isWiFiConnected() && isMQTTConnected()) { 
                // Publica los datos
                publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20);
                //sendStoredData();
                // Guardar datos con bandera 1 (para enviar)
                //saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
            } else {
                Serial.println("Sin conexión - Datos guardados para envío posterior");
                saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
            }
            lastPublishTime = xTaskGetTickCount();
        }    
        
        //vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(3000));  
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }  
}

void TaskSendStoredData(void *pvParameters) {
    while (true) {

        // Espera notificación para ejecutar
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        Serial.println("Tarea de envío activada");

        // Seguridad básica
        if (isWiFiConnected() && isMQTTConnected()) {
            //sendStoredData();
        } else {
            Serial.println("No hay conexión, envío pospuesto");
        }

        // Pequeño respiro
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


void setup() {
    Serial.begin(9600); 
    initPublishing();
    // Crear el semáforo
    wifiSemaphore = xSemaphoreCreateBinary();
    lcdSemaphore= xSemaphoreCreateBinary();
    spiffsMutex = xSemaphoreCreateMutex();
    if (wifiSemaphore == NULL) {
        Serial.println("Error al crear el semáforo");
    }
    xTaskCreatePinnedToCore(
        conectToInternet,           // Función de la tarea
        "conect to Internet",       // Nombre de la tarea
        7000,            // Tamaño de la pila
        NULL,            // Parámetros de la tarea
        1,               // Prioridad de la tarea
        &Task1Handle,1);   // Handle de la tarea

    xTaskCreatePinnedToCore(
        Task2,           // Función de la tarea
        "Tarea 2",       // Nombre de la tarea
        6000,            // Tamaño de la pila
        NULL,            // Parámetros de la tarea
        2,               // Prioridad de la tarea
        &Task2Handle,0);   // Handle de la tarea
    xTaskCreatePinnedToCore(
        TaskSendStoredData,
        "Send Stored Data",
        6000,
        NULL,
        1,
        &TaskSendHandle,1);

}

void loop() {
    // Nada aquí, todo se maneja en las tareas
}