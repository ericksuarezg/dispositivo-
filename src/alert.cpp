#include <Arduino.h>
#include "mqtt.h"
#include "timeSetUp.h"

String lowRange = "Bajo";
String highRange = "Alto";
String externTemp = "Temperatura Ambiente";
String externHumidity = "Humedad Relativa";
String externTempDS18 = "Temperatura DS18B20";


// Estructura para almacenar los límites de las alertas
struct AlertLimits {
    float tempDHTMin = 0.0;
    float tempDHTMax = 0.0;
    float humidityMin = 0.0;
    float humidityMax = 0.0;
    float tempDS18Min = 0.0;
    float tempDS18Max = 0.0;
};

AlertLimits alertLimits;

// Función para configurar los límites de las alertas
void configurarAlertas(float tempDHTMin, float tempDHTMax, 
                      float humidityMin, float humidityMax,
                      float tempDS18Min, float tempDS18Max) {
    alertLimits.tempDHTMin = tempDHTMin;
    alertLimits.tempDHTMax = tempDHTMax;
    alertLimits.humidityMin = humidityMin;
    alertLimits.humidityMax = humidityMax;
    alertLimits.tempDS18Min = tempDS18Min;
    alertLimits.tempDS18Max = tempDS18Max;
}

// Función para verificar y generar alertas
void verificarAlertas(float tempDHT, float humidity, float tempDS18) {
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

    // Verificar temperatura DS18B20
    if (tempDS18 < alertLimits.tempDS18Min) {
        Serial.println("¡ALERTA! Temperatura DS18B20 muy baja: " + String(tempDS18) + "°C");
        publishAlerts(getDateSeparate(), getTimeSeparate(), lowRange, externTempDS18, tempDS18);
    } else if (tempDS18 > alertLimits.tempDS18Max) {
        Serial.println("¡ALERTA! Temperatura DS18B20 muy alta: " + String(tempDS18) + "°C");
        publishAlerts(getDateSeparate(), getTimeSeparate(), highRange, externTempDS18, tempDS18);
    }
}

void alertaOutSensorService(String sensorName, String messageSensorOutService){
    publishAlertsSensorOutService(getDateSeparate(), getTimeSeparate(), messageSensorOutService, sensorName);
}
