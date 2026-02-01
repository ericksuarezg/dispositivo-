#include "PublishingInit.h"

Preferences nvs;
uint32_t lastSequence = 0;
String sessionId = "";

// Inicialización de NVS y sesión
void initPublishing() {
    nvs.begin("esp32data", false);
    lastSequence = nvs.getUInt("lastSeq", 0); // recuperar última secuencia
    sessionId = "ESP32-" + String(ESP.getEfuseMac(), HEX) + "-" + String(millis());
    Serial.println("Session ID: " + sessionId);
}

