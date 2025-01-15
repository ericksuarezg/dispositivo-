#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LcdSetup.h>
#include <ArduinoJson.h>
#include <WiFiManagerSetUp.h>

// Configuración del cliente seguro
WiFiClientSecure espClientSecure;
PubSubClient client(espClientSecure);

// Configuración del servidor MQTT
const char* mqtt_server = "goblue.com.co";
const char* mqtt_user = "Termo5263";
const char* mqtt_password = "Termo1234";
const char* mqtt_client_id = "6679e920f060820300eb69e4";
const int mqtt_port = 8884; // Cambiar al puerto para MQTTS

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
    displayInfoOnLCD("Mensaje recibido", payloadStr.c_str());
}

void reconnect(SemaphoreHandle_t lcdSemaphore) {
    Serial.print("El estado de la conexión MQTT es: ");
    Serial.println(client.state());

    if (!client.connected() && isWiFiConnected()) {
        Serial.print("Intentando conexión al servidor MQTT seguro...");
        espClientSecure.setInsecure();
        if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
            Serial.println("Conectado al servidor MQTTs!");
            mqttConnected = true;
            client.subscribe(mqtt_user);

            if (xSemaphoreTake(lcdSemaphore, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                displayInfoOnLCD("Intentando", "conexión MQTTs");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                displayInfoOnLCD("Conectado a", mqtt_server);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                xSemaphoreGive(lcdSemaphore);
            }
        } else {
            Serial.print("Fallo, rc=");
            Serial.print(client.state());
            Serial.println(". Intentando nuevamente en 5 segundos...");
            if (xSemaphoreTake(lcdSemaphore, 5000 / portTICK_PERIOD_MS) == pdTRUE) {
                displayInfoOnLCD("Intentando MQTTs", "nuevamente en 5 seg");
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                xSemaphoreGive(lcdSemaphore);
            }
        }
    } else {
        Serial.println("MQTT ya estaba conectado.");
    }
}
void CheckForMessages(){
  client.loop();
}

void mqttSetUp(SemaphoreHandle_t lcdSemaphore) {
    // Configura el cliente seguro para conexiones sin validación de certificados
    espClientSecure.setInsecure();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    client.setKeepAlive(60); // Configura un Keep-Alive de 60 segundos

    if (!client.connected() && isWiFiConnected()) {
        Serial.print("Intentando conexión al servidor MQTT seguro...");
        displayInfoOnLCD("Intentando", "conexión MQTTs");
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
            Serial.println("Conectado al servidor MQTTs!");
            mqttConnected = true;
            client.subscribe(mqtt_user);
            displayInfoOnLCD("Conectado a", mqtt_server);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xSemaphoreGive(lcdSemaphore);
        } else {
            Serial.print("Fallo, rc=");
            Serial.print(client.state());
            Serial.println(". Intentando nuevamente en 5 segundos...");
            displayInfoOnLCD("Intentando MQTTs", "nuevamente en 5 seg");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xSemaphoreGive(lcdSemaphore);
        }
    }
}

void publishData(SemaphoreHandle_t lcdSemaphore, float temperaturaDHT, float humedadRelativa, float temperaturaDS18) {
    if (isnan(temperaturaDHT) || isnan(humedadRelativa) || isnan(temperaturaDS18) || temperaturaDS18 == -127) {
        Serial.println("Error: Datos inválidos. No se publicará información.");
        if (xSemaphoreTake(lcdSemaphore, portMAX_DELAY) == pdTRUE) {
            displayInfoOnLCD("Error publicación", "Datos no válidos");
            xSemaphoreGive(lcdSemaphore);
        }
        return;
    }

    DynamicJsonDocument jsonDoc(256);
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

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    client.publish(mqtt_client_id, jsonString.c_str());
    if (xSemaphoreTake(lcdSemaphore, 3000 / portTICK_PERIOD_MS) == pdTRUE) {
        displayDataOnLCD(temperaturaDHT, humedadRelativa, temperaturaDS18);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        displayInfoOnLCD("Datos enviados", "MQTTs");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
    }
}

bool isMQTTConnected() {
    return client.connected();
}
void storagePublishData(float temperaturaDHT,float humedadRelativa, float temperaturaDS18) {
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
}