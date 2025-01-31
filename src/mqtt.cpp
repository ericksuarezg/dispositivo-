#include <WiFiManager.h>
#include <PubSubClient.h>
#include <LcdSetup.h>
#include <ArduinoJson.h>
#include <WiFiManagerSetUp.h> 
#include <timeSetUp.h>
#include <storage.h>
#include <WiFiClientSecure.h>

#define MAX_BUFFER_SIZE 512  // Modifica este valor según lo que necesites


//WiFiClient espClient;
//PubSubClient client(espClient);
WiFiClientSecure espClientSecure;
PubSubClient client(espClientSecure);

//const char * mqtt_server= "192.168.18.10";// local
const char * mqtt_server= "goblue.com.co";
const char* mqtt_user = "NeveraPlayaAlta"; 
//const char* mqtt_user = "Termo8936"; 
const char* mqtt_password = "Nevera2025";
//const char* mqtt_password = "Termo2023";
const char* mqtt_client_id = "679be8d3541507c16065c542";
//const char* mqtt_client_id = "6680422a40a2bf513dbce2df";
//const int mqtt_port = 7080;
const int mqtt_port = 8884; // mqtts
//const int mqtt_port = 3251; // puerto local

bool mqttConnected = false;


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Mensaje recibido en el tópico: " + String(topic));
  Serial.print("Contenido: ");
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadStr += (char)payload[i];
  }
  Serial.println();
  displayInfoOnLCD("Mensaje recibido",payloadStr.c_str());
}

void reconnect(SemaphoreHandle_t lcdSemaphore) {
  Serial.print("el estado de la conexion mqtt es ");
  Serial.print(client.state());
  if (client.state()!=MQTT_CONNECTED) {
    if (!client.connected()  && isWiFiConnected() ) {
      Serial.print("Intentando conexión al servidor MQTT...");
        if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
          Serial.println("Conectado al servidor MQTT!");
          mqttConnected = true;
          client.subscribe(mqtt_user);
          

          if (xSemaphoreTake(lcdSemaphore,2000/portTICK_PERIOD_MS)==pdTRUE){
            displayInfoOnLCD("   Intentando"," coneccion MQTT");
            vTaskDelay(2000/ portTICK_PERIOD_MS);
            displayInfoOnLCD("   Conectado a",  mqtt_server);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            xSemaphoreGive(lcdSemaphore); 
          }
        } else {
          Serial.print("Fallo, rc=");
          Serial.print(client.state());
          Serial.println(" Intentando nuevamente en 5 segundos...");
          if (xSemaphoreTake(lcdSemaphore,5000/portTICK_PERIOD_MS)==pdTRUE){
            displayInfoOnLCD("intentando MQTT","nuevamente en 7 seg");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xSemaphoreGive(lcdSemaphore);
          }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  } else {
    Serial.println("MQTT ya estaba conectado.");
    //displayInfoOnLCD("WiFi: Conectado", "MQTT: Conectado");
    //vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
 

void CheckForMessages(){
  client.loop();
}

void mqttSetUp(SemaphoreHandle_t lcdSemaphore){
  espClientSecure.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(60); // Configura un Keep-Alive de 60 segundos
  client.setBufferSize(MAX_BUFFER_SIZE);

  if (!client.connected() && isWiFiConnected() ) {
    Serial.print("Intentando conexión al servidor MQTT...");
    displayInfoOnLCD("   Intentando"," coneccion MQTT");
    vTaskDelay(3000/ portTICK_PERIOD_MS);
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("Conectado al servidor MQTT!");
      mqttConnected = true;
      client.subscribe(mqtt_user);
      //client.subscribe("devices/" + mqtt_client_id + "/configuration");   // Nueva suscripción
      displayInfoOnLCD("   Conectado a",  mqtt_server);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore); 
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos...");
      displayInfoOnLCD("intentando MQTT","nuevamente en 5 seg");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
    }
  }
}

bool publishData(String date, String time, float temperaturaDHT, float humedadRelativa, float temperaturaDS18) {
    Serial.print(temperaturaDS18);
    delay(100);

    // Verificar si los datos son válidos
    if (isnan(temperaturaDHT) || isnan(humedadRelativa) || isnan(temperaturaDS18) || temperaturaDS18 == -127) {
        Serial.println("Error: Datos inválidos. No se publicará información.");
        return false;
    }

    // Convertir mqtt_client_id a String si no es ya un String
    String clientId = String(mqtt_client_id);

    // Crear manualmente el JSON como cadena
    String jsonString = "{";
    jsonString += "\"typeMessage\":\"messageCurrent\",";
    jsonString += "\"deviceId\":\"" + clientId + "\",";
    jsonString += "\"data\":{";
    jsonString += "\"header\":[";
    jsonString += "\"Fecha lectura\",";
    jsonString += "\"Hora de lectura\",";
    jsonString += "\"Temperatura Ambiente\",";
    jsonString += "\"Humedad Ambiente\",";
    jsonString += "\"Temperatura Nevera\"";
    jsonString += "],";
    jsonString += "\"body\":[";
    jsonString += "\"" + date + "\",";
    jsonString += "\"" + time + "\",";
    jsonString += String(temperaturaDHT) + ",";
    jsonString += String(humedadRelativa) + ",";
    jsonString += String(temperaturaDS18);
    jsonString += "]";
    jsonString += "}";
    jsonString += "}";

    // Imprimir la cadena JSON que se enviará
    Serial.println("ESTO ES LO QUE VOY A ENVIAR: " + jsonString);

    // Publicar el mensaje en el tema deseado
    Serial.println("Tamaño del mensaje JSON: " + String(jsonString.length()));
    bool result = client.publish(mqtt_client_id, jsonString.c_str());

    if (result) {
        Serial.println("Mensaje publicado correctamente.");
        return true;
    } else {
        Serial.println("Error al publicar el mensaje.");
        return false;
    }
}



bool isMQTTConnected() {
    return client.connected();
}


void publishAlerts(String datePart, String timePart, String rangeTipe, String varName, float alertVatiable) {
    // Verificar si hay conexión MQTT
    if (!client.connected()) {
        Serial.println("Error: No hay conexión MQTT para publicar alertas");
        return;
    }

    // Crear el JSON de alertas
    String jsonString = "{";
    jsonString += "\"typeMessage\":\"alertsMessage\",";
    jsonString += "\"deviceId\":\"" + String(mqtt_client_id) + "\",";
    jsonString += "\"data\":{";
    jsonString += "\"header\":[";
    jsonString += "\"Fecha lectura\",";
    jsonString += "\"Hora de lectura\",";
    jsonString += "\"Tipo de rango\",";
    jsonString += "\"Variable\","; 
    jsonString += "\"Valor\"";
    jsonString += "],";
    jsonString += "\"body\":[";
    jsonString += "\"" + datePart + "\",";
    jsonString += "\"" + timePart + "\",";
    jsonString += "\"" + rangeTipe + "\",";
    jsonString += "\"" + varName + "\",";
    jsonString += String(alertVatiable);
    jsonString += "]";
    jsonString += "}";
    jsonString += "}";

    Serial.println("Publicando alertas:");
    Serial.println(jsonString);
    client.publish(mqtt_client_id, jsonString.c_str());
}

void publishAlertsSensorOutService(String datePart, String timePart, String messageSensorOutService, String sensorName) {
    // Verificar si hay conexión MQTT
    if (!client.connected()) {
        Serial.println("Error: No hay conexión MQTT para publicar alertas");
        return;
    }

    String jsonString = "{";
    jsonString += "\"typeMessage\":\"alertsMessage\",";
    jsonString += "\"deviceId\":\"" + String(mqtt_client_id) + "\",";
    jsonString += "\"data\":{";
    jsonString += "\"header\":[";
    jsonString += "\"Fecha lectura\",";
    jsonString += "\"Hora de lectura\",";
    jsonString += "\"Mensaje\",";
    jsonString += "\"Nombre Sensor\"";
    jsonString += "],";
    jsonString += "\"body\":[";
    jsonString += "\"" + datePart + "\",";
    jsonString += "\"" + timePart + "\",";
    jsonString += "\"" + messageSensorOutService + "\",";
    jsonString += "\"" + sensorName + "\"";
    jsonString += "]";
    jsonString += "}";
    jsonString += "}";

    Serial.println("Publicando alerta de sensor fuera de servicio:");
    Serial.println(jsonString);
    client.publish(mqtt_client_id, jsonString.c_str());
}


