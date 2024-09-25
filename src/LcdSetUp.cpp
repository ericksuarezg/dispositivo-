#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setUpLcd(SemaphoreHandle_t wifiSemaphore) {
  //xSemaphoreTake(wifiSemaphore,(TickType_t) 1000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Inicialilzando ");
  lcd.setCursor(0,1);
  lcd.print("  sistema..... ");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  xSemaphoreGive(wifiSemaphore);
}

void displayInfoOnLCD(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayDataOnLCDofDHT(float temperature, float humidity) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp DTH: ");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("HR: ");
  lcd.print(humidity);
  lcd.print("%");
}

void displayDataOnLCDofDbs18b20(float temperatureCDs18b20) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp DS18:");
  lcd.setCursor(0,1);
  lcd.print(temperatureCDs18b20);
  lcd.print(" C");
}
void displayDataOnLCD(float temperature, float humidity,float temperatureCDs18b20) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp DTH: ");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("HR: ");
  lcd.print(humidity);
  lcd.print("%");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp DS18:");
  lcd.print(temperatureCDs18b20);
  lcd.print(" C");
  lcd.setCursor(0,1);
  lcd.print("HR:");
  lcd.print(humidity);
  lcd.print("%");
}
