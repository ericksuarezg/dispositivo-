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
const char * mqtt_server= "aguisu.com";
//const char* mqtt_user = "Studio23"; // camila ramos 
const char* mqtt_user = "Termo8936"; // gladis prueba 
//const char* mqtt_user = "LicethFig2025"; //liceht figeroa (annie) 
//const char* mqtt_user = "Fabionoe2025"; // fabio noe 
//const char* mqtt_user = "Labgiovana2025"; // giovana savogal 
//const char* mqtt_user = "TermoArmonia2025*"; // Armonia Dental  
//const char* mqtt_user = "Convexa001"; // juan pablo ramirez  
//const char* mqtt_user = "Termoliceth01"; // liceth lara  
//const char* mqtt_user = "Diana2025"; // Diana Aguilar  
//const char* mqtt_user = "Santamaria2025"; // Santamaria 
//const char* mqtt_user = "Mendez01"; // Mauricio Mendez  
//const char* mqtt_password = "Studio23"; // camila Ramos 
const char* mqtt_password = "Termo2023";//gladis prueba
//const char* mqtt_password = "LicethFig2025";//liceth figueroa (Annie)
//const char* mqtt_password = "Fabionoe2025";//fabio noe
//const char* mqtt_password = "Labgiovana2025";//giovana savogal
//const char* mqtt_password = "TermoArmonia2025*";//Armonia dental
//const char* mqtt_password = "Convexa001";//juan pablo ramirez
//const char* mqtt_password = "Termoliceth01";//liceth lara
//const char* mqtt_password = "Diana2025";//Diana Aguilar
//const char* mqtt_password = "Santamaria2025";//SantaMaria
//const char* mqtt_password = "Mendez01";// mauricio mendez
//const char* mqtt_client_id = "679a32069fe20cce0f68ad9f";// camila Ramos
const char* mqtt_client_id = "6680422a40a2bf513dbce2df";// Gladis prueba 
//const char* mqtt_client_id = "682e9194a921582ebf58d455";// liceth figeroa  
//const char* mqtt_client_id = "67f6d26fa921582ebf5851f3";// fabio noe 
//const char* mqtt_client_id = "67f6d431a921582ebf585211";// giovana savogal 
//const char* mqtt_client_id = "680e3684a921582ebf588adb";// Armonia dental
//const char* mqtt_client_id = "67dbfc15a921582ebf582359";// juan pablo ramirez 
//const char* mqtt_client_id = "67e3ef32a921582ebf5834c1";// liceth lara
//const char* mqtt_client_id = "67dd4aeda921582ebf5824f4";// Diana Aguilar
//const char* mqtt_client_id = "67c4a1d22a4d94d4af7d43be";// Santa Maria
//const char* mqtt_client_id = "67a8f727dabe7a7d86f62150";// Mauricio Mendez  
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
     // xSemaphoreGive(lcdSemaphore); 
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando nuevamente en 5 segundos...");
      displayInfoOnLCD("intentando MQTT","nuevamente en 5 seg");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
     // xSemaphoreGive(lcdSemaphore);
    }
  }
  xSemaphoreGive(lcdSemaphore); 
}

bool publishData(String date, String time, float temperaturaDHT, float humedadRelativa, float temperaturaDS18) {
    Serial.print(temperaturaDS18);
    vTaskDelay(100 / portTICK_PERIOD_MS);

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
    jsonString += "\"temperatura Almacen\",";
    jsonString += "\"humedad Almacen\",";
    jsonString += "\"temperatura Nevera\"";
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
/* bool publishData(String date, String time, float temperaturaDHT, float humedadRelativa, float temperaturaDS18) {
    Serial.print(temperaturaDS18);
    delay(100);

    // Verificar si los datos son válidos
    if (isnan(temperaturaDHT) || isnan(humedadRelativa) || isnan(temperaturaDS18) || temperaturaDS18 == -127) {
        Serial.println("Error: Datos inválidos. No se publicará información.");
        return false;
    }

    // Crear un buffer de caracteres en lugar de usar String dinámico
    char jsonBuffer[256];  // Ajustar tamaño si es necesario
    snprintf(jsonBuffer, sizeof(jsonBuffer), 
        "{\"typeMessage\":\"messageCurrent\",\"deviceId\":\"%s\",\"data\":{\"header\":[\"Fecha lectura\",\"Hora de lectura\",\"temperatura Almacen\",\"humedad Almacen\",\"temperatura Nevera\"],\"body\":[\"%s\",\"%s\",%.2f,%.2f,%.2f]}}",
        mqtt_client_id, date.c_str(), time.c_str(), temperaturaDHT, humedadRelativa, temperaturaDS18);

    // Imprimir la cadena JSON que se enviará
    Serial.println("ESTO ES LO QUE VOY A ENVIAR: ");
    Serial.println(jsonBuffer);

    // Publicar el mensaje en el tema deseado
    Serial.println("Tamaño del mensaje JSON: " + String(strlen(jsonBuffer)));
    bool result = client.publish(mqtt_client_id, jsonBuffer);

    if (result) {
        Serial.println("Mensaje publicado correctamente.");
    } else {
        Serial.println("Error al publicar el mensaje.");
    }
    return result;
} */



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


