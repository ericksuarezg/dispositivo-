#include <Arduino.h>
#include <WiFiManagerSetUp.h>
#include <LcdSetUp.h>
<<<<<<< HEAD
#include <mqtt.h> 
=======
//#include <mqtt.h>
#include <mqttSecurity.h>
>>>>>>> 7a6985da34a88d4fa20d8a1e7fc09c8246a3f5e8
#include <dthSetUp.h>
#include <ds18b20SetUp.h>
#include "freertos/semphr.h"
#include <storage.h>



// Definir los "handle" de las tareas
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;

// Declarar el semáforo para WiFi
SemaphoreHandle_t wifiSemaphore;
// declarar semaforo para lcd 
SemaphoreHandle_t lcdSemaphore;
// Función para la primera tarea
void conectToInternet(void *pvParameters) {
    setUpWifi(wifiSemaphore,lcdSemaphore);
    mqttSetUp(lcdSemaphore);
    //xSemaphoreGive(wifiSemaphore);
    while (true) {
        Serial.println("verificando conexion a Wifi y Mqtt en ejecucion");
        reconectWiFi(lcdSemaphore);
        reconnect(lcdSemaphore);
        CheckForMessages();
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Espera de 1 segundo
    }
}


// Función para la segunda tarea
void Task2(void *pvParameters) {
    float temperaturaDHT;
    float humedad;
    float temperatureCDs18b20;
    setupSPIFFS();
    setUpLcd(wifiSemaphore);
    dthSensorsetUp();
    ds18b20SetUp(lcdSemaphore);

    unsigned long lastPublishTime = millis();
    unsigned long publishInterval = 60000;


    unsigned long lastSaveTime = millis();
    unsigned long saveInte = 20000;
    
     
    while (true) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Espera de 2 segundos
        Serial.println("ejecutando lectura de sensores");
        ds18b20ReadTemperature(lcdSemaphore,temperatureCDs18b20);
        dhtReading(lcdSemaphore,temperaturaDHT,humedad);

        if (millis() - lastSaveTime >= saveInte){
            saveDataToCSV(temperaturaDHT,humedad,temperatureCDs18b20,publishInterval,1);  
            lastSaveTime = millis();  
        }else if (millis() - lastSaveTime < saveInte){
            saveDataToCSV(temperaturaDHT,humedad,temperatureCDs18b20,publishInterval,0);
        }
        
        if (isWiFiConnected() && isMQTTConnected()) {
            if (millis() - lastPublishTime >= publishInterval) {
                sendStoredData(lcdSemaphore);
                //publishData(lcdSemaphore, temperaturaDHT, humedad, temperatureCDs18b20);  
                lastPublishTime = millis();  
            }
        } 
        vTaskDelay(1000 / portTICK_PERIOD_MS) ; 
    }     
}

void setup() {
    Serial.begin(9600); 
    // Crear las tareas
    // Crear el semáforo
    wifiSemaphore = xSemaphoreCreateBinary();
    lcdSemaphore= xSemaphoreCreateBinary();
    if (wifiSemaphore == NULL) {
        Serial.println("Error al crear el semáforo");
    }
    xTaskCreatePinnedToCore(
        conectToInternet,           // Función de la tarea
        "conect to Internet",       // Nombre de la tarea
        5000,            // Tamaño de la pila
        NULL,            // Parámetros de la tarea
        1,               // Prioridad de la tarea
        &Task1Handle,1);   // Handle de la tarea

     xTaskCreatePinnedToCore(
        Task2,           // Función de la tarea
        "Tarea 2",       // Nombre de la tarea
        5000,            // Tamaño de la pila
        NULL,            // Parámetros de la tarea
        1,               // Prioridad de la tarea
        &Task2Handle,0);   // Handle de la tarea 
}

void loop() {
    // Nada aquí, todo se maneja en las tareas
}