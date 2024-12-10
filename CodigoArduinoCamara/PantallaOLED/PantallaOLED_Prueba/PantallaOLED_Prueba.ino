#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  oled.display();
  delay(2000);
}

void loop() {
  for(int i = 0; i < 21; i++)
  {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 10);
    oled.println(F("Hola, Mundo!"));
    oled.setCursor(0, 40);
    oled.println(F("Contador:"));
    oled.setTextSize(2);
    oled.setCursor(70,40);
    oled.println(i);
    oled.display();
    delay(1000);
  }

}
