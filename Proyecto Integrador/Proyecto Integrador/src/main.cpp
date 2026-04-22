#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <Servo.h>
#include <Arduino.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP280 bmp;
Servo servoVentilacion;

const int PIN_LED = 9;
const int PIN_SERVO = 10;
const int PIN_POT = A0;

unsigned long ultimoReporte = 0;
bool modoManual = false;
bool estadoManualAbierto = false;

void setup() {
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);
    
    servoVentilacion.attach(PIN_SERVO);
    servoVentilacion.write(0);

    Serial.println("\n--- SISTEMA INTEGRAL DE CLIMATIZACION ---");
    dht.begin();
    
    // Inicializamos el sensor I2C (Lab 06)
    if (!bmp.begin()) {
        Serial.println("Ojo: BMP180 no detectado, revisa cables I2C.");
    } else {
        Serial.println("Sensores DHT22 y BMP180 inicializados (I2C OK).");
    }
    
    Serial.println("Listo. Escriba 'AYUDA' para comandos UART.");
}

void loop() {
    // 1. LECTURA DE SENSORES
    float tempActual = dht.readTemperature();
    float presionActual = bmp.readPressure() / 100.0; // Convertir Pa a hPa
    
    if (isnan(tempActual)) tempActual = 40.0; // Valor de rescate

    int valorADC = analogRead(PIN_POT);
    float tempUmbral = map(valorADC, 0, 1023, 20, 40); // Termostato 20-40°C

    // 2. LÓGICA DE CONTROL (PWM y SERVO)
    if (!modoManual) {
        if (tempActual >= tempUmbral) {
            servoVentilacion.write(90); // Abre ventilación
            analogWrite(PIN_LED, 255);  // Alarma activada
        } else {
            servoVentilacion.write(0);  // Cierra ventilación
            analogWrite(PIN_LED, 0);    
        }
    } else {
        if (estadoManualAbierto) {
            servoVentilacion.write(90);
            analogWrite(PIN_LED, 50); // Brillo medio para modo manual
        } else {
            servoVentilacion.write(0);
            analogWrite(PIN_LED, 0);
        }
    }

    // 3. COMUNICACIÓN UART (ESCUCHA)
    if (Serial.available()) {
        String comando = Serial.readStringUntil('\n');
        comando.trim();
        comando.toUpperCase();

        if (comando == "ABRIR") {
            modoManual = true; estadoManualAbierto = true;
            Serial.println(">> MODO MANUAL: VENTANA FORZADA A ABIERTA");
        } else if (comando == "CERRAR") {
            modoManual = true; estadoManualAbierto = false;
            Serial.println(">> MODO MANUAL: VENTANA FORZADA A CERRADA");
        } else if (comando == "AUTO") {
            modoManual = false;
            Serial.println(">> SISTEMA EN MODO AUTOMATICO");
        } else if (comando == "AYUDA") {
            Serial.println("\n--- MENU ---");
            Serial.println("ABRIR, CERRAR, AUTO");
        }
    }

    // 4. TELEMETRÍA UART
    if (millis() - ultimoReporte > 2000) {
        ultimoReporte = millis();
        Serial.print("Temp: "); Serial.print(tempActual, 1);
        Serial.print("C | Presion: "); Serial.print(presionActual, 1);
        Serial.print(" hPa | Termostato: "); Serial.print(tempUmbral, 1);
        Serial.println(modoManual ? "C | [MANUAL]" : "C | [AUTO]");
    }
}