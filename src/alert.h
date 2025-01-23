#ifndef ALERT_H
#define ALERT_H

// Función para configurar los límites de las alertas
void configurarAlertas(float tempDHTMin, float tempDHTMax, 
                      float humidityMin, float humidityMax,
                      float tempDS18Min, float tempDS18Max);

// Función para verificar y generar alertas
void verificarAlertas(float tempDHT, float humidity, float tempDS18);

#endif // ALERT_H
