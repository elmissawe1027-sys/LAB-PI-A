#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Pines de control específicos para Arduino UNO (según tu manual)
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8
// Nota de hardware: El pin MOSI debe ir al pin 11 y SCK al pin 13.

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Colores custom
#define GRIS    0x8410
#define AZUL_M  0x2471

int frameCount = 0;

// Declaración estricta de funciones para C++
void dibujarPanel();
void actualizarDatos(float temp, float hum);

void setup() {
    Serial.begin(115200);
    
    tft.begin();
    tft.setRotation(1);  // Pantalla en horizontal
    tft.fillScreen(ILI9341_BLACK); 

    // Pantalla de bienvenida
    tft.setTextColor(ILI9341_CYAN);
    tft.setTextSize(2);
    tft.setCursor(20, 20);
    tft.println("Lab 08 - SPI TFT");
    
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setCursor(20, 50);
    tft.println("Arduino UNO | PlatformIO");
    delay(1500);

    tft.fillScreen(ILI9341_BLACK);
    dibujarPanel();
}

void loop() {
    frameCount++;
    
    // Simulamos los datos del sensor
    float temperatura = 24.5 + sin(frameCount * 0.05) * 3.0;
    float humedad     = 58.0 + cos(frameCount * 0.03) * 5.0;
    
    actualizarDatos(temperatura, humedad);
    
    delay(500);
}

void dibujarPanel() {
    tft.fillRect(0, 0, 320, 30, AZUL_M);
    tft.setTextColor(ILI9341_WHITE); 
    tft.setTextSize(1);
    tft.setCursor(10, 10); 
    tft.print("ESTACION AMBIENTAL | ARDUINO UNO");
    
    tft.drawRoundRect(10, 40, 140, 100, 8, ILI9341_CYAN);
    tft.setTextColor(ILI9341_CYAN); 
    tft.setCursor(20, 55); 
    tft.print("TEMPERATURA");
    
    tft.drawRoundRect(165, 40, 140, 100, 8, ILI9341_GREEN);
    tft.setTextColor(ILI9341_GREEN); 
    tft.setCursor(175, 55); 
    tft.print("HUMEDAD");
    
    tft.drawRect(10, 155, 300, 75, GRIS);
    tft.setTextColor(GRIS); 
    tft.setCursor(15, 160); 
    tft.print("Historial temp.");
}

void actualizarDatos(float temp, float hum) {
    // Borramos el dato anterior con un cuadro negro para no repintar toda la pantalla
    tft.fillRect(15, 70, 125, 45, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE); 
    tft.setTextSize(3);
    tft.setCursor(20, 78);
    tft.print(temp, 1); 
    tft.print(" C");
    
    tft.fillRect(170, 70, 125, 45, ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE); 
    tft.setTextSize(3);
    tft.setCursor(175, 78);
    tft.print(hum, 1); 
    tft.print(" %");
}