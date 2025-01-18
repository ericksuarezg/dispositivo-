#include <WiFiManager.h>
#include <PubSubClient.h>
#include <LcdSetup.h>
#include <ArduinoJson.h>
#include <WiFiManagerSetUp.h>  
#include <timeSetUp.h>
#include <storage.h>

#define MAX_BUFFER_SIZE 512  // Modifica este valor según lo que necesites


WiFiClient espClient;
PubSubClient client(espClient);

//const char * mqtt_server= "192.168.18.10";// local
const char * mqtt_server= "goblue.com.co";
const char* mqtt_user = "Equipo01"; 
const char* mqtt_password = "Device01";
const char* mqtt_client_id = "677ebe3f7fc72fa1fe5a2fde";
const int mqtt_port = 7080;
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

void publishData(SemaphoreHandle_t lcdSemaphore, String date, String time, float temperaturaDHT, float humedadRelativa, float temperaturaDS18) {
    Serial.print(temperaturaDS18);
    delay(5000);

    // Verificar si los datos son válidos
    if (isnan(temperaturaDHT) || isnan(humedadRelativa) || isnan(temperaturaDS18) || temperaturaDS18 == -127) {
        Serial.println("Error: Datos inválidos. No se publicará información.");
        if (xSemaphoreTake(lcdSemaphore, portMAX_DELAY) == pdTRUE) {
            displayInfoOnLCD("Error publicación", "Datos no válidos");
            xSemaphoreGive(lcdSemaphore);
        }
        return;
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
    jsonString += "\"temperatura dth22\",";
    jsonString += "\"humedad Relativa\",";
    jsonString += "\"temperatura ds18b20\"";
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
    } else {
        Serial.println("Error al publicar el mensaje.");
        return;
    }

    // Mostrar los datos en el LCD
    if (xSemaphoreTake(lcdSemaphore, 3000 / portMAX_DELAY) == pdTRUE) {
        displayDataOnLCD(temperaturaDHT, humedadRelativa, temperaturaDS18);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        displayInfoOnLCD("La data ha sido", "     enviada    ");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
    }
}



bool isMQTTConnected() {
    return client.connected();
}

void storagePublishData(String datePart, String timePart, float temperaturaDHT,float humedadRelativa, float temperaturaDS18) {
  // Crear un objeto JSON para almacenar los datos
  Serial.print(temperaturaDS18);
  delay(10);
  if (isnan(temperaturaDHT) || isnan(humedadRelativa) || isnan(temperaturaDS18) || temperaturaDS18==-127) {
    Serial.println("Error: Datos inválidos. No se publicará información.");
    return; 
  }

  DynamicJsonDocument jsonDoc(256); // Ajusta el tamaño según sea necesario
  jsonDoc["typeMessage"] = "messageCurrent";
  jsonDoc["deviceId"] = mqtt_client_id;
  JsonObject data = jsonDoc.createNestedObject("data");
  JsonArray header = data.createNestedArray("header");
  header.add("Fecha Lectura");
  header.add("Hora de lectura");
  header.add("temperatura dth22");
  header.add("humedad Relativa");
  header.add("temperatura ds18b20");
  JsonArray body = data.createNestedArray("body");
  body.add(datePart);
  body.add(timePart);
  body.add(temperaturaDHT);
  body.add(humedadRelativa);
  body.add(temperaturaDS18);

  // Obtener los datos de temperatura y humedad
  const char* partDate = body[0];
  const char* partTime= body[1];
  const char* temperatureDTH = body[2];
  const char* humidity = body[3];
  const char* temperatureDs18= body[4];

  // Serializar el JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Ver el JSON que se va a publicar en el monitor serial
  Serial.println(F("Datos que se publicarán:"));
  Serial.println(jsonString); // Imprimir JSON serializado
  // Publicar el mensaje en el tema deseado
  client.publish(mqtt_client_id, jsonString.c_str());
  // Mostrar los datos en el LCD
}