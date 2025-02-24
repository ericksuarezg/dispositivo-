#include <Arduino.h>
#include <WiFiManagerSetUp.h>
#include <LcdSetUp.h>
#include <mqtt.h>
#include <dthSetUp.h>
#include <timeSetUp.h>
#include "freertos/semphr.h" 
#include <storage.h>
#include <alert.h>


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
        UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
        Serial.print("Pila libre en conectToInternet: ");
        Serial.println(freeStack);
        Serial.println("verificando conexion a Wifi y Mqtt en ejecucion");
        Serial.print("Heap libre en conectToInternet: ");
        Serial.println(ESP.getFreeHeap());
        reconectWiFi(lcdSemaphore);
        reconnect(lcdSemaphore);
        sendStoredData();
        CheckForMessages();
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Espera de 1 segundo
    }
}


// Función para la segunda tarea
void Task2(void *pvParameters) {
    float temperaturaDHT;
    float humedad;
    float tempDHTMin = 15;
    float tempDHTMax = 41;
    float humidityMin = 40;
    float humidityMax = 98;

    setupSPIFFS();
    setUpLcd(wifiSemaphore);
    dthSensorsetUp(lcdSemaphore);
    configurarAlertas(tempDHTMin, tempDHTMax, humidityMin, humidityMax);
    
    unsigned long lastPublishTime = millis();
    unsigned long publishInterval = 600000; // 10 en minutos
    //unsigned long publishInterval = 30000; // 4 horas en milisegundos

    unsigned long lastSaveTime = millis();
    unsigned long saveInterval = 3000000; // 5 minutos en milisegundos

    while (true) {
        UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL);
        Serial.print("Pila libre en Task2: ");
        Serial.println(freeStack);
        Serial.print("Heap libre en Task2: ");
        Serial.println(ESP.getFreeHeap());
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Espera de 2 segundos
        Serial.println("ejecutando lectura de sensores");
        dhtReading(lcdSemaphore,temperaturaDHT,humedad);
        updateClockDisplay(lcdSemaphore);
          // Almacenar datos periódicamente
        unsigned long currentTime = millis();
        // Verificar intervalo de guardado
        if (currentTime - lastSaveTime >= saveInterval) {
            // Verificar Alertas
            //saveDataToCSV(payload,getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
            verificarAlertas(temperaturaDHT, humedad);
            lastSaveTime = currentTime;
        }
        /*
        // Verificar intervalo de publicación
        if (currentTime - lastPublishTime >= publishInterval) {
            // Verificar alertas
            verificarAlertas(temperaturaDHT, humedad);

            
            // Verificar conexión antes de publicar
            if (isWiFiConnected() && isMQTTConnected()) {
                // Publica los datos
                publishData(getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad);
                //sendStoredData();
                // Guardar datos con bandera 1 (para enviar)
                //saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, temperatureCDs18b20, 0);
                lastPublishTime = currentTime;
            } else {
                Serial.println("Sin conexión - Datos guardados para envío posterior");
                saveDataToCSV(payload, getDateSeparate(), getTimeSeparate(), temperaturaDHT, humedad, 0);
            }
            
            lastPublishTime = currentTime;
        }    
        */
        
        vTaskDelay(1000 / portTICK_PERIOD_MS) ;  
    }  
}

void setup() {
    Serial.begin(9600); 
    // Crear el semáforo
    wifiSemaphore = xSemaphoreCreateBinary();
    lcdSemaphore= xSemaphoreCreateBinary();
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
        7000,            // Tamaño de la pila
        NULL,            // Parámetros de la tarea
        2,               // Prioridad de la tarea
        &Task2Handle,0);   // Handle de la tarea 
}

void loop() {
    // Nada aquí, todo se maneja en las tareas
}