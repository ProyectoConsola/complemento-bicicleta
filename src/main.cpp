// #include<ESP32Time.h>
#include <Arduino.h>
#include "fabgl.h"
#include <iostream>
#include <stdlib.h>
#define MAX_V  6

using fabgl::iclamp;
using std::string;

fabgl::VGAController DisplayController;
fabgl::Canvas canvas(&DisplayController);

const int pinEncoder = 12;
unsigned int cntPulsos;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 12;

// Funcion de interrupcion
void IRAM_ATTR contadorPulsos_ISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    cntPulsos++;
    lastDebounceTime = millis();
  }
}


const uint8_t barra[]{
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
Bitmap bitmapV = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(79, 245, 7));
Bitmap bitmapVL = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(185, 245, 7));
Bitmap bitmapA = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(245, 200, 7));
Bitmap bitmapN = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(255, 153, 0));
Bitmap bitmapR = Bitmap(28, 30, barra, PixelFormat::Mask, RGB888(249, 28, 28));



Sprite sprites[5];

struct SpeedOmeter : public Scene
{
  static const int TEXTROWS = 4;
  static const int TEXT_X = 130;
  static const int TEXT_Y = 122;

  float distanciaM=0, distanciaT=0;
  float distanciaPulsos=0, Vmps=0;
  unsigned int cntTiempo=0, bloqueTiempo=0;
  float MaxV = MAX_V;
  char strDistancia[20];
  char strVelocidad[20];

  SpeedOmeter()
      : Scene(0, 250, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight())
  {
  }

  void init()
  {
    canvas.setBrushColor(Color::Black);

    
    canvas.selectFont(&fabgl::FONT_6x8);
    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));
    
    
    // Agregar sprites :D >:b
    sprites[0].addBitmap(&bitmapV);
    sprites[1].addBitmap(&bitmapVL);
    sprites[2].addBitmap(&bitmapA);
    sprites[3].addBitmap(&bitmapN);
    sprites[4].addBitmap(&bitmapR);
    
    sprites[0].moveTo(260, 150);
    sprites[1].moveTo(260, 120);
    sprites[2].moveTo(260, 90);
    sprites[3].moveTo(260, 60);
    sprites[4].moveTo(260, 30);

    sprites[0].visible = true;
    sprites[1].visible = false;
    sprites[2].visible = false;
    sprites[3].visible = false;
    sprites[4].visible = false;

    DisplayController.setSprites(sprites, 5);
  }
  void update(int updateCount)
  {
    // Serial.println(updateCount);
    cntTiempo = millis() - bloqueTiempo;
    if (cntTiempo >= 1000)
    {
      bloqueTiempo += cntTiempo;
      distanciaM = ((cntPulsos / 16.0) * 2 * PI * 0.32);
      distanciaT += distanciaM;
      Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667;
      
      snprintf(strDistancia, 20, "%.2g", distanciaT);
      snprintf(strVelocidad, 20, "%.2g", Vmps);
      cntPulsos = 0;
    }

    canvas.clear();
    
    canvas.setPenColor(128, 128, 128);
    canvas.drawRectangle(257, 27, 290, 183);
    
    canvas.setPenColor(255,255,255);
    canvas.drawText(15, 60, "Distancia (m): ");
    canvas.drawText(15, 80, strDistancia);
    canvas.drawText(15, 100, "Velocidad (Km/h): ");
    canvas.drawText(15, 120, strVelocidad);

    if(Vmps > 0)
    {
      sprites[0].visible= true;
    }
    else
    {
      sprites[0].visible= false;
    }
    if(Vmps > MaxV/5){
      sprites[1].visible= true;
    }
    else
    {
      sprites[1].visible= false;
    }
    if(Vmps > MaxV/4){
      sprites[2].visible= true;
    }
    else
    {
      sprites[2].visible= false;
    }
    if(Vmps > MaxV/3){
      sprites[3].visible= true;
    }
    else
    {
      sprites[3].visible= false;
    }
    if(Vmps > MaxV/2){
      sprites[4].visible= true;
    }
    else
    {
      sprites[4].visible= false;
    }
    
    
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