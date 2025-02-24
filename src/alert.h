#ifndef ALERT_H
#define ALERT_H

// Función para configurar los límites de las alertas
void configurarAlertas(float tempDHTMin, float tempDHTMax, 
                      float humidityMin, float humidityMax);

// Función para verificar y generar alertas
void verificarAlertas(float tempDHT, float humidity);

void alertaOutSensorService(String sensorName, String messageSensorOutService);

#endif // ALERT_H
