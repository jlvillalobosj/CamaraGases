#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 8
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//Declarar LCD y pines
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(7,6,5,4,3,2);

// Variables para guardar los últimos valores válidos
float lastTemp = 0.0;
float lastHum = 0.0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2); //Definir las dimensiones del LCD (16x2)
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

 //Seleccionamos en que columna y en que linea empieza a mostrar el texto
 lcd.setCursor(0,0);
 lcd.print("T:");
 lcd.print(lastTemp,0);
 lcd.print("C  H:");
 lcd.print(lastHum,1); 
 lcd.print("%");

 lcd.setCursor(0,1);
 lcd.print("CH4:");
 lcd.print(lastTemp,1);
 lcd.print("  CO2:");
 lcd.print(lastHum,1);


  delay(1000); // Esperar 1 segundo antes de la siguiente lectura
 
}
