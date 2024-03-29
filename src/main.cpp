//#include<ESP32Time.h>
#include<Arduino.h>
const int pinEncoder = 2; 
volatile int cntPulsos = 0; 

volatile float distanciaM=0;

//ESP32Time rtc(3600);

void IRAM_ATTR contadorPulsos_ISR() {
  cntPulsos++;
  
  
}

void setup() {
  Serial.begin(115200);
  pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), contadorPulsos_ISR, RISING); //o CHANGE
  //rtc.setTime(1609459200);
}

void loop() {
  
  //Revisar los pulsos de encoder por revolucion, son los 200.0
  float distanciaPulsos = cntPulsos / 200.0;

  distanciaM = distanciaM + (distanciaPulsos * 2 * PI * 30)/100; //30 son el radio de la rueda
  
  //float Vmps = distanciaM / rtc.getSecond();

  Serial.print("Distancia pulsos: ");
  Serial.println(distanciaPulsos);
  Serial.print("Distancia (m): ");
  Serial.println(distanciaM);
  //Serial.print("Velocidad (m/s): ");
  //Serial.println(Vmps);
 
  
  cntPulsos = 0;

  delay(1000);
}
