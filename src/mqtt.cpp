#include <WiFiManager.h>
#include <PubSubClient.h>
#include <LcdSetup.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);
const char * mqtt_server= "23.239.4.166";
const char* mqtt_user = "Termo5263";
const char* mqtt_password = "Termo1234";
const char* mqtt_client_id = "6679e920f060820300eb69e4";
const int mqtt_port = 7080;


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
    while (!client.connected()) {
      Serial.print("Intentando conexión al servidor MQTT...");
      if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
        Serial.println("Conectado al servidor MQTT!");
        mqttConnected = true;
        client.subscribe("Termo5263");
        xSemaphoreTake(lcdSemaphore, portMAX_DELAY);
        displayInfoOnLCD("WiFi: Conectado", "MQTT: Conectado");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore); 
      } else {
        Serial.print("Fallo, rc=");
        Serial.print(client.state());
        Serial.println(" Intentando nuevamente en 5 segundos...");
        xSemaphoreTake(lcdSemaphore, portMAX_DELAY);
        displayInfoOnLCD("WiFi: Conectado", "MQTT: Desconectado");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
      }
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
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //reconnect(lcdSemaphore);
  while (!client.connected()) {
    Serial.print("Intentando conexión al servidor MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("Conectado al servidor MQTT!");
      mqttConnected = true;
      client.subscribe("Termo5263");
      displayInfoOnLCD("WiFi: Conectado", "MQTT: Conectado");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore); 
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos...");
      displayInfoOnLCD("WiFi: Conectado", "MQTT: Desconectado");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
    }
  }
}

void publishData(SemaphoreHandle_t lcdSemaphore, float temperaturaDHT,float humedadRelativa, float temperaturaDS18) {
  // Crear un objeto JSON para almacenar los datos
  DynamicJsonDocument jsonDoc(256); // Ajusta el tamaño según sea necesario

  jsonDoc["typeMessage"] = "messageCurrent";
  jsonDoc["deviceId"] = "6679e920f060820300eb69e4";
  //jsonDoc["deviceId"] = "65d1689bf8f75e518b8057a7b";
 // jsonDoc["deviceId"] = "65d1689bf8f75e518b8057a7b";
  JsonObject data = jsonDoc.createNestedObject("data");
  JsonArray header = data.createNestedArray("header");
  header.add("temperatura dth22");
  header.add("humedad Relativa");
  header.add("temperatura ds18b20");
  JsonArray body = data.createNestedArray("body");
  body.add(temperaturaDHT);
  body.add(humedadRelativa);
  body.add(temperaturaDS18);

  // Obtener los datos de temperatura y humedad
  const char* temperatureDTH = body[0];
  const char* humidity = body[1];
  const char* temperatureDs18= body[2];
  // Mostrar los datos en el LCD
  xSemaphoreTake(lcdSemaphore, portMAX_DELAY);
  displayDataOnLCD(temperaturaDHT,humedadRelativa,temperaturaDS18);
  
  // Serializar el JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Publicar el mensaje en el tema deseado
  client.publish("6679e920f060820300eb69e4", jsonString.c_str());
  displayInfoOnLCD("La data ha sido","     enviada    ");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xSemaphoreGive(lcdSemaphore);
}
