#include <Arduino.h>
#include <Servo.h>

// Definición de pines según el manual del laboratorio
const int PIN_LED   = 9;    // Pin PWM para el LED
const int PIN_SERVO = 10;   // Pin PWM para el servomotor
const int PIN_POT   = A0;   // Entrada analógica del potenciómetro

Servo miServo;
bool modoAuto = true;
String bufferCmd = "";

// Prototipos de funciones (Obligatorios en C++ puro)
void procesarComando(String cmd);

void setup() {
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);
    
    // Inicializar el servomotor
    miServo.attach(PIN_SERVO);
    miServo.write(90); // Posición neutral
    
    Serial.println("\nLab 07 - PWM + Servo + ADC");
    Serial.println("Comandos: D:0-255 | S:0-180 | AUTO | POT");
}

void loop() {
    // Lectura de comandos vía Serial
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') { 
            procesarComando(bufferCmd); 
            bufferCmd = ""; 
        } 
        else if (c != '\r') {
            bufferCmd += c;
        }
    }

    if (modoAuto) {
        // Modo Automático: Efecto respiración y barrido
        static int val = 0;
        static int dir = 1;
        static unsigned long ultimo = 0;
        
        if (millis() - ultimo > 10) {
            ultimo = millis();
            val += dir * 2;
            if (val >= 255) { val = 255; dir = -1; }
            if (val <= 0)   { val = 0;   dir =  1; }
            
            analogWrite(PIN_LED, val);
            int grados = map(val, 0, 255, 0, 180);
            miServo.write(grados);
        }
    } else {
        // Modo Manual: Potenciómetro controla servo y LED
        int lecturaPot = analogRead(PIN_POT);
        int grados = map(lecturaPot, 0, 1023, 0, 180);
        int brillo  = map(lecturaPot, 0, 1023, 0, 255);
        
        miServo.write(grados);
        analogWrite(PIN_LED, brillo);
        
        static unsigned long ultimoReport = 0;
        if (millis() - ultimoReport > 500) {
            ultimoReport = millis();
            float voltaje = lecturaPot * (5.0 / 1023.0);
            Serial.print("ADC="); Serial.print(lecturaPot);
            Serial.print(" | V="); Serial.print(voltaje, 2);
            Serial.print(" | Servo="); Serial.print(grados);
            Serial.print("deg | LED="); Serial.println(brillo);
        }
    }
}

void procesarComando(String cmd) {
    cmd.trim();
    if (cmd == "AUTO") { 
        modoAuto = true; 
        Serial.println("[OK] Modo automático"); 
    }
    else if (cmd == "POT") { 
        modoAuto = false; 
        Serial.println("[OK] Modo potenciómetro"); 
    }
    else if (cmd.startsWith("D:")) {
        int v = cmd.substring(2).toInt();
        v = constrain(v, 0, 255);
        analogWrite(PIN_LED, v);
        modoAuto = false;
        Serial.print("[OK] LED PWM="); Serial.println(v);
    }
    else if (cmd.startsWith("S:")) {
        int g = cmd.substring(2).toInt();
        g = constrain(g, 0, 180);
        miServo.write(g);
        modoAuto = false;
        Serial.print("[OK] Servo="); Serial.print(g); Serial.println(" grados");
    }
}