// #include<ESP32Time.h>
#include <Arduino.h>
#include "fabgl.h"
#include <iostream>
#include <stdlib.h>

using fabgl::iclamp;
using std::string;

fabgl::VGAController DisplayController;
fabgl::Canvas canvas(&DisplayController);

const int pinEncoder = 12;
unsigned int cntPulsos = 0, cntTiempo = 0, bloqueTiempo;
float distanciaM = 0, distanciaT = 0;
float distanciaPulsos, Vmps;
unsigned long tAct = 0, tAnt = 0;
float tDif = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 12;

// Funcion de interrupcion
void IRAM_ATTR contadorPulsos_ISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    cntPulsos++;
    tAct = micros();
  }
}

struct SpeedOmeter : public Scene
{
  static const int TEXTROWS = 4;
  static const int TEXT_X = 130;
  static const int TEXT_Y = 122;

  SpeedOmeter()
      : Scene(0, 250, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight())
  {
  }

  void init()
  {
    canvas.setBrushColor(Color::Black);
    // Agregar sprites :D >:b
  }
  void update(int updateCount)
  {
    // Serial.println(updateCount);
    

    canvas.clear();
    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    canvas.selectFont(&fabgl::FONT_6x8);
    canvas.setPenColor(Color::BrightWhite);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    canvas.drawText(15, 60, "Distancia (m): ");
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(0));

    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    canvas.selectFont(&fabgl::FONT_6x8);
    canvas.setPenColor(Color::BrightWhite);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    canvas.drawText(15, 100, "Velocidad (Km/h): ");
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(0));
    canvas.setBrushColor(Color::Black);

    cntTiempo = millis() - bloqueTiempo;
    if (cntTiempo >= 1000)
    {
      bloqueTiempo += cntTiempo;
      distanciaM = ((cntPulsos / 16.0) * 2 * PI * 0.32);
      distanciaT += distanciaM;
      Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667;
      cntPulsos = 0;
    }

    char strDistancia[20];
    char strVelocidad[20];
    snprintf(strDistancia, 20, "%g", distanciaT);

    snprintf(strVelocidad, 20, "%g", Vmps);

    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    canvas.selectFont(&fabgl::FONT_6x8);
    canvas.setPenColor(Color::BrightWhite);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    canvas.drawText(220, 60, strDistancia);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(0));

    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    canvas.selectFont(&fabgl::FONT_6x8);
    canvas.setPenColor(Color::BrightWhite);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    canvas.drawText(220, 100, strVelocidad);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(0));
  }

  void collisionDetected(Sprite *spriteA, Sprite *spriteB, Point collisionPoint)
  {
  }
};



void setup()
{
  Serial.begin(115200);
  pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), contadorPulsos_ISR, RISING);
  DisplayController.begin();
  DisplayController.setResolution(VGA_320x200_75Hz);
  Serial.println("¡Listo!");
}

void loop()
{

  SpeedOmeter speedOmeter;
  speedOmeter.start();
  // mediciones en el encoder
  /*
  cntTiempo = millis() - bloqueTiempo;
  if (cntTiempo >= 1000)
  {
    bloqueTiempo += cntTiempo;

    distanciaM = ((cntPulsos / 16.0) * 2 * PI * 0.32);
    distanciaT += distanciaM;
    Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667;
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
  */
  delay(1);
}