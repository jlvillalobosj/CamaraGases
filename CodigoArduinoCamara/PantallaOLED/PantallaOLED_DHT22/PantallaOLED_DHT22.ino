#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define DHTPIN 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables para guardar los últimos valores válidos
float lastTemp = 0.0;
float lastHum = 0.0;

void setup() {
  Serial.begin(9600);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  oled.display();
  delay(2000);

  dht.begin(); // Inicializa el sensor DHT
}

void loop() {
  // Leer humedad y temperatura
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Verificar si las lecturas son válidas (no NaN)
  if (!isnan(h)) {
    lastHum = h; // Actualizar la última lectura válida de humedad
  }
  if (!isnan(t)) {
    lastTemp = t; // Actualizar la última lectura válida de temperatura
  }

  // Mostrar las lecturas en el serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(lastHum);
  Serial.print(F("%  Temperature: "));
  Serial.print(lastTemp);
  Serial.println(F("°C "));

  // Actualizar la pantalla OLED
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  
  oled.setCursor(0, 5);
  oled.println(F("Temp:"));
  oled.setCursor(35, 5);
  oled.println(lastTemp, 1); // Mostrar con 1 decimal
  
  oled.setCursor(65, 5);
  oled.println(F("Hum:"));
  oled.setCursor(90, 5);
  oled.println(lastHum, 1); // Mostrar con 1 decimal
  
  oled.display();
  delay(1000); // Esperar 1 segundo antes de la siguiente lectura
}
