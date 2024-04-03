//#include<ESP32Time.h>
#include<Arduino.h>
const int pinEncoder = 2;
unsigned int cntPulsos = 0, cntTiempo = 0, bloqueTiempo;
float distanciaM = 0, distanciaT = 0;
float distanciaPulsos, Vmps;
unsigned long tAct = 0, tAnt = 0;
float tDif = 0;

unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 12;  


//Funcion de interrupcion
void IRAM_ATTR contadorPulsos_ISR() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    cntPulsos++;
    tAct = micros();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), contadorPulsos_ISR, RISING);

}

void loop() {


  //mediciones en el encoder
  cntTiempo = millis() - bloqueTiempo;
  if (cntTiempo >= 1000) {
    bloqueTiempo += cntTiempo;

    distanciaM = ((cntPulsos / 16.0) * 2 * PI * 0.32);
    distanciaT += distanciaM;
    Vmps = distanciaM / (cntTiempo / 1000.0)*3.66667;
    Serial.print("Cant pulsos: ");
    Serial.println(cntPulsos);
    Serial.print("Distancia pulsos: ");
    Serial.println(distanciaPulsos);
    Serial.print("Distancia (m): ");
    Serial.println(distanciaT);
    Serial.print("Velocidad (km/h): ");
    Serial.println(Vmps);

    cntPulsos = 0;
  }
  delay(1);
}