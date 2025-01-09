#include <WiFiManager.h>
#include <PubSubClient.h>
#include <LcdSetup.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);
const char * mqtt_server= "aguisu.com";
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
    float currentTime= millis();
    float temporiTime=30000;
    while (!client.connected()) {
      if((millis()-currentTime)>=temporiTime){
        if (xSemaphoreTake(lcdSemaphore,3000/ portMAX_DELAY) == pdTRUE){
          displayInfoOnLCD(" Tiempo agotado"," conexion MQTT");
          vTaskDelay(5000 / portTICK_PERIOD_MS);
          displayInfoOnLCD("Reintento en ..."," 30 segundos...");
          vTaskDelay(5000 / portTICK_PERIOD_MS);
          xSemaphoreGive(lcdSemaphore);   
        }
        return; 
      }
      if (xSemaphoreTake(lcdSemaphore, portMAX_DELAY) == pdTRUE) {  
        Serial.print("Intentando conexión al servidor MQTT...");
        if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
          Serial.println("Conectado al servidor MQTT!");
          mqttConnected = true;
          client.subscribe(mqtt_user);
          displayInfoOnLCD("   Intentando"," coneccion MQTT");
          vTaskDelay(3000/ portTICK_PERIOD_MS);
          displayInfoOnLCD("   Conectado a",  mqtt_server);
          vTaskDelay(5000 / portTICK_PERIOD_MS);
          xSemaphoreGive(lcdSemaphore); 
          vTaskDelay(5000 / portTICK_PERIOD_MS);
        } else {
          Serial.print("Fallo, rc=");
          Serial.print(client.state());
          Serial.println(" Intentando nuevamente en 5 segundos...");
          displayInfoOnLCD("intentando MQTT","nuevamente en 5 seg");
          vTaskDelay(5000 / portTICK_PERIOD_MS);
          xSemaphoreGive(lcdSemaphore);
          vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
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
    displayInfoOnLCD("   Intentando"," coneccion MQTT");
    vTaskDelay(3000/ portTICK_PERIOD_MS);
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("Conectado al servidor MQTT!");
      mqttConnected = true;
      client.subscribe(mqtt_user);
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

void publishData(SemaphoreHandle_t lcdSemaphore, float temperaturaDHT,float humedadRelativa, float temperaturaDS18) {
  // Crear un objeto JSON para almacenar los datos
  Serial.print(temperaturaDS18);
  delay(5000);
  if (isnan(temperaturaDHT) || isnan(humedadRelativa) || isnan(temperaturaDS18) || temperaturaDS18==-127) {
    Serial.println("Error: Datos inválidos. No se publicará información.");
    if (xSemaphoreTake(lcdSemaphore, portMAX_DELAY) == pdTRUE) {
       displayInfoOnLCD("Error publicación", "Datos no válidos");
       xSemaphoreGive(lcdSemaphore);
    }
    return; 
  }
  DynamicJsonDocument jsonDoc(256); // Ajusta el tamaño según sea necesario
  jsonDoc["typeMessage"] = "messageCurrent";
  jsonDoc["deviceId"] = mqtt_client_id;
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

  // Serializar el JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  // Publicar el mensaje en el tema deseado
  client.publish(mqtt_client_id, jsonString.c_str());
  // Mostrar los datos en el LCD
  if (xSemaphoreTake(lcdSemaphore, 3000/ portMAX_DELAY)==pdTRUE)
  {
    displayDataOnLCD(temperaturaDHT,humedadRelativa,temperaturaDS18);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    displayInfoOnLCD("La data ha sido","     enviada    ");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xSemaphoreGive(lcdSemaphore);
  }
}

bool isMQTTConnected() {
    return client.connected();
}