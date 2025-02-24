#include <dht.h> 
#include <LcdSetUp.h>
#define DHT_PIN 32


static bool dhtConfigured = false;
dht DHT;

bool dthSensorsetUp(SemaphoreHandle_t lcdSemaphore) {
    Serial.println("\U0001F504 Configurando sensor DHT22...");
   
    // Mostrar en LCD
    displayInfoOnLCD("  Configurando", "     DHT22");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Intentar primera lectura para validar sensor
    int chk = DHT.read22(DHT_PIN);
   
    if (chk != DHTLIB_OK) {
        Serial.println("\u274C Error: No se pudo configurar el sensor DHT22.");
        displayInfoOnLCD("Error:", "Sensor DHT22");
       
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        dhtConfigured = false;
        xSemaphoreGive(lcdSemaphore);
        return false;
    }

    // Sensor funcionando correctamente, muestra los datos
    float humidity = DHT.humidity;
    float temperature = DHT.temperature;
   
    Serial.println("\u2705 Sensor DHT22 configurado correctamente.");
    Serial.print("\U0001F321️ Temperatura: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("\U0001F4A7 Humedad: "); Serial.print(humidity); Serial.println(" %");

    displayInfoOnLCD("Sensor DTH", "OK");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    displayDataOnLCDofDHT(temperature, humidity);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Calcular sensación térmica (heat index)
    float heatIndex = temperature + (0.5 * (1.1 * humidity - 10));
    char termicSen[16];
    snprintf(termicSen, sizeof(termicSen), "%.1f C", heatIndex);

    displayInfoOnLCD("Sens. térmica:", termicSen);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Marcar el sensor como configurado correctamente
    dhtConfigured = true;
    xSemaphoreGive(lcdSemaphore);
    return true;
}

void dhtReading(SemaphoreHandle_t lcdSemaphore, float &temperaturaDHT, float &humedad) {
    if (!dhtConfigured) {
        Serial.println("Sensor DHT22 no configurado. Intentando configurar nuevamente.");
        if (xSemaphoreTake(lcdSemaphore, 3000 / portTICK_PERIOD_MS) == pdTRUE) {
            dhtConfigured = dthSensorsetUp(lcdSemaphore);
            xSemaphoreGive(lcdSemaphore);
        }
        if (!dhtConfigured) {
            Serial.println("Error: No se pudo configurar el sensor DHT22.");
            //temperaturaDHT = NAN;
            //humedad = NAN; // Valores no válidos para indicar error
            return;
        } else {
            if (xSemaphoreTake(lcdSemaphore, 3000 / portTICK_PERIOD_MS) == pdTRUE) {
                displayInfoOnLCD(" Config exitosa", "de sensor DTH22");
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                xSemaphoreGive(lcdSemaphore);
                return;
            }
        }
    }
    
    int intentos = 0;
    int chk;
    do {
        chk = DHT.read22(DHT_PIN);
        if (chk == DHTLIB_OK) {
            break;
        }
        Serial.println("⚠️ Error al leer el sensor DHT22, reintentando...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        intentos++;
    } while (intentos < 5);
    
    if (chk != DHTLIB_OK) {
        Serial.println("❌ Fallo en la lectura del sensor DHT22 tras 5 intentos.");
        if (xSemaphoreTake(lcdSemaphore, 3000 / portTICK_PERIOD_MS) == pdTRUE) {
            displayInfoOnLCD("error DTH", "Not reading it");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            xSemaphoreGive(lcdSemaphore);
        }
        //temperaturaDHT = NAN;
        //humedad = NAN;
        return;
    }

    temperaturaDHT = DHT.temperature;
    humedad = DHT.humidity;
    Serial.print("\U0001F321️ Temperatura DHT: "); Serial.println(temperaturaDHT);
    Serial.print("\U0001F4A7 Humedad: "); Serial.println(humedad);
    
    if (xSemaphoreTake(lcdSemaphore, 3000 / portTICK_PERIOD_MS) == pdTRUE) {
        displayDataOnLCDofDHT(temperaturaDHT, humedad);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
    }
}


float dhtGetTemperature() {
    for (int i = 0; i < 3; i++) {  // Intentar hasta 3 veces
        int chk = DHT.read22(DHT_PIN);
        if (chk == DHTLIB_OK) {
            return DHT.temperature;
        }
        Serial.println("⚠️ Error en la lectura de temperatura, reintentando...");
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Esperar 2 segundos antes de reintentar
    }
    Serial.println("❌ Fallo en la lectura de temperatura tras 3 intentos.");
    return NAN;
}

float dhtGetHumidity() {
    for (int i = 0; i < 3; i++) {  // Intentar hasta 3 veces
        int chk = DHT.read22(DHT_PIN);
        if (chk == DHTLIB_OK) {
            return DHT.humidity;
        }
        Serial.println("⚠️ Error en la lectura de humedad, reintentando...");
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Esperar 2 segundos antes de reintentar
    }
    Serial.println("❌ Fallo en la lectura de humedad tras 3 intentos.");
    return NAN;
}

/*bool dthSensorsetUp (SemaphoreHandle_t lcdSemaphore){
   displayInfoOnLCD("  Configurando","     DHT22");
   vTaskDelay(3000/ portTICK_PERIOD_MS);
   dht.begin();
   float humidity = dht.readHumidity();
   float temperature = dht.readTemperature();
 
   if (isnan(humidity) || isnan(temperature)) {
     // Error al leer el sensor
     displayInfoOnLCD("Error:", "Sensor DHT22");
     Serial.println("Error: No se pudo configurar el sensor DHT22.");
     dhtConfigured = false;
     vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
     return false;
   } else {
     // Sensor funcionando correctamente, muestra los datos
     displayInfoOnLCD("Sensor DTH","OK");
     vTaskDelay(2000/ portTICK_PERIOD_MS);
     displayDataOnLCDofDHT(temperature,humidity);
     vTaskDelay(2000/ portTICK_PERIOD_MS);
     //calculamos la sensacion termica
     float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
     char termicSen[16];
     snprintf(termicSen, sizeof(termicSen), "%.1f C", heatIndex);
     displayInfoOnLCD("Sens. termica de",termicSen);
     dhtConfigured = true;
     vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
     return true;
   }
  
}
void dhtReading(SemaphoreHandle_t lcdSemaphore,float &temperaturaDHT, float &humedad) {
  if (!dhtConfigured) {
    Serial.println("Sensor DHT22 no configurado. Intentando configurar nuevamente.");
    if (xSemaphoreTake(lcdSemaphore,3000 / portTICK_PERIOD_MS)==pdTRUE){
      dhtConfigured = dthSensorsetUp(lcdSemaphore);
      //xSemaphoreGive(lcdSemaphore);
    }
    if (!dhtConfigured)
    {
      Serial.println("Error: No se pudo configurar el sensor DHT22.");
      temperaturaDHT = NAN;
      humedad = NAN; // Valores no válidos para indicar error
      return;
    }else{
      if (xSemaphoreTake(lcdSemaphore,3000 / portTICK_PERIOD_MS)==pdTRUE)
      {
       displayInfoOnLCD(" Config exitosa", "de sensor DTH22");
       vTaskDelay(3000 / portTICK_PERIOD_MS);
       xSemaphoreGive(lcdSemaphore);
       return;
      }
    }
  }  
  //temperaturaDHT = dht.readTemperature();
 // humedad = dht.readHumidity();
  Serial.println("TemperaturaDTH");
  Serial.print(temperaturaDHT);
  Serial.println("Humedad");
  Serial.print(humedad);
  
  if (isnan(temperaturaDHT) || isnan(humedad)) {
    if (xSemaphoreTake(lcdSemaphore, 3000 / portTICK_PERIOD_MS) == pdTRUE) {
      Serial.println("Error al leer el sensor DHT22");
      displayInfoOnLCD("error DTH", "Not reading it");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      xSemaphoreGive(lcdSemaphore);
    }
  } else {
    bool alerta = false;

    // Mostrar datos normales si no hay alerta
    if (!alerta) {
      if (xSemaphoreTake(lcdSemaphore, 3000 /portTICK_PERIOD_MS) == pdTRUE) {
        displayDataOnLCDofDHT(temperaturaDHT, humedad);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xSemaphoreGive(lcdSemaphore);
      }
    }
  }
}

float dhtGetTemperature(){
  return dht.readTemperature();
}

float dhtGetHumidity(){
  return dht.readHumidity();
}
*/
  
