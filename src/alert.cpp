#include <Arduino.h>
#include "mqtt.h"
#include "timeSetUp.h"

String lowRange = "Bajo";
String highRange = "Alto";
String externTemp = "Temperatura Ambiente";
String externHumidity = "Humedad Relativa";


// Estructura para almacenar los límites de las alertas
struct AlertLimits {
    float tempDHTMin = 0.0;
    float tempDHTMax = 0.0;
    float humidityMin = 0.0;
    float humidityMax = 0.0;
};

AlertLimits alertLimits;

// Función para configurar los límites de las alertas
void configurarAlertas(float tempDHTMin, float tempDHTMax, 
                      float humidityMin, float humidityMax) {
    alertLimits.tempDHTMin = tempDHTMin;
    alertLimits.tempDHTMax = tempDHTMax;
    alertLimits.humidityMin = humidityMin;
    alertLimits.humidityMax = humidityMax;
}

// Función para verificar y generar alertas
void verificarAlertas(float tempDHT, float humidity) {
    // Verificar temperatura DHT
    if (tempDHT < alertLimits.tempDHTMin) {
        
        Serial.println("¡ALERTA! Temperatura DHT muy baja: " + String(tempDHT) + "°C");
        publishAlerts(getDateSeparate(), getTimeSeparate(), lowRange, externTemp, tempDHT);
    } else if (tempDHT > alertLimits.tempDHTMax) {
        Serial.println("¡ALERTA! Temperatura DHT muy alta: " + String(tempDHT) + "°C");
        publishAlerts(getDateSeparate(), getTimeSeparate(), highRange, externTemp, tempDHT);
    }

    // Verificar humedad
    if (humidity < alertLimits.humidityMin) {
        Serial.println("¡ALERTA! Humedad muy baja: " + String(humidity) + "%");
        publishAlerts(getDateSeparate(), getTimeSeparate(), lowRange, externHumidity, humidity);
    } else if (humidity > alertLimits.humidityMax) {
        Serial.println("¡ALERTA! Humedad muy alta: " + String(humidity) + "%");
        publishAlerts(getDateSeparate(), getTimeSeparate(), highRange, externHumidity, humidity);
    }

}

void alertaOutSensorService(String sensorName, String messageSensorOutService){
    publishAlertsSensorOutService(getDateSeparate(), getTimeSeparate(), messageSensorOutService, sensorName);
}
