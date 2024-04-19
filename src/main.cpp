// #include<ESP32Time.h>
#include <Arduino.h>
#include "fabgl.h"
#include "fabutils.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <Ps3Controller.h>
#define MAX_V  40

using fabgl::iclamp;

using std::string;

fabgl::VGAController DisplayController;
fabgl::Canvas canvas(&DisplayController);

//Variables del encoder
const int pinEncoder = 12;
unsigned int cntPulsos;


float distanciaT;
char velocidad[20];
char distancia[20];
char potencia[20];

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 12;

uint64_t currentTime;
uint64_t lastInputRecieved;
constexpr uint64_t INPUT_DELAY = 200;

// Funcion de interrupcion
void IRAM_ATTR contadorPulsos_ISR()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    cntPulsos++;
    lastDebounceTime = millis();
  }
}

//sprites de barras
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
  /*static const int TEXTROWS = 4;
  static const int TEXT_X = 130;
  static const int TEXT_Y = 122;*/
  float MaxV = MAX_V;

  float distanciaM = 0.0, distanciaPulsos = 0.0;
  float Vmps=0.0, potenciaf=0.0, vart = 0.0;
  unsigned int cntTiempo=0, bloqueTiempo=0;
  bool speedChange = false;
  
  char strDistancia[20];
  char strVelocidad[20];
  char strPotencia[20];

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
    sprites[1].visible = sprites[2].visible = sprites[3].visible = sprites[4].visible = false;

    DisplayController.setSprites(sprites, 5);
  }
  void update(int updateCount)
  {

    if (Ps3.data.analog.stick.rx > 40 || Ps3.data.analog.stick.rx < -40)
    {
      if (Ps3.data.analog.stick.rx > 0)
      {
        if (Vmps <= MaxV - 1.0)
          Vmps = Vmps + 1.0;
      }
      if (Ps3.data.analog.stick.rx < 0)
      {
        if (Vmps >= 1.0)
          Vmps = Vmps - 1.0;
      }
    }
        

    // Serial.println(updateCount);
    if(vart != 0.0 && millis() - vart >= 90000){
      sprites[0].visible = sprites[1].visible = sprites[2].visible = sprites[3].visible = sprites[4].visible = false;
      canvas.clear();
      stop();
    }
    else{

      cntTiempo = millis() - bloqueTiempo;
      if (cntTiempo >= 1000)
      {
        bloqueTiempo += cntTiempo;

        distanciaM = ((Vmps*1000)/3600) * (cntTiempo / 1000.0);
        distanciaT += distanciaM;

        /*
        distanciaM = ((cntPulsos / 20.0) * 2 * PI * 0.32);
        distanciaT += distanciaM;
        Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667;*/ // 3.6667 es para pasar los m/s a km/h



        potenciaf= Vmps*2.73;
        if(Vmps >= 1.0 && speedChange==false){
          speedChange=true;
          vart= millis();
        }
        
        snprintf(strDistancia, 20, "%4.4g", distanciaT);
        snprintf(strVelocidad, 20, "%.2g", Vmps);
        snprintf(strPotencia, 20, "%.2g", potenciaf);

        sprintf(distancia, "%.2g", distanciaT);
        Serial.println((String)"Distancia:" + distancia);
        sprintf(velocidad, "%.2g", Vmps);
        Serial.println((String)"Vmps:" + velocidad);
        sprintf(potencia, "%.2g", potencia);
        //Serial.println((String)"Potencia:" + potencia);

        cntPulsos = 0;
      }

      canvas.clear();
      
      //dibujar rectanugulo
      canvas.setPenColor(128, 128, 128);
      canvas.drawRectangle(257, 27, 290, 183);

      canvas.selectFont(&fabgl::FONT_6x9);
      canvas.setPenColor(140, 210, 245);
      canvas.drawText(10, 10, "MEDIDOR DE VELOCIDAD");

      canvas.selectFont(&fabgl::FONT_6x8);
      canvas.setPenColor(255,255,255);
      canvas.drawText(15, 40, "Distancia (m): ");
      canvas.drawText(15, 60, strDistancia);
      canvas.drawText(15, 80, "Velocidad (Km/h): ");
      canvas.drawText(15, 100, strVelocidad);
      canvas.drawText(15, 120, "Potencia (W): ");
      canvas.drawText(15, 140, strPotencia);
      //Condiciones de velocidad
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
      if(Vmps > MaxV/2.3){
        sprites[3].visible= true;
      }
      else
      {
        sprites[3].visible= false;
      }
      if(Vmps > MaxV/1.5){
        sprites[4].visible= true;
      }
      else
      {
        sprites[4].visible= false;
      }
    
    }
  
  }
  void collisionDetected(Sprite *spriteA, Sprite *spriteB, Point collisionPoint)
  {
  }
  
};


struct FinalScene : public Scene
{
  /*static const int TEXTROWS = 4;
  static const int TEXT_X   = 130;
  static const int TEXT_Y   = 122;
  */
  char strDistancia[20];


  FinalScene()
  : Scene(0, 20, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight())
  {
  }

  void init(){
    canvas.clear();
    canvas.selectFont(&fabgl::FONT_9x18);
    canvas.setPenColor(255, 255, 255);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(5));
    canvas.drawText(20, 20, "SE HA ACABADO EL");
    canvas.drawText(50,50, "EL TIEMPO !!!");
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));

    canvas.selectFont(&fabgl::FONT_8x13);
    canvas.drawText(30, 100, "Distancia total: ");
    snprintf(strDistancia, 20, "%.4g", distanciaT);
    canvas.drawText(100, 120, strDistancia);
    canvas.setGlyphOptions(GlyphOptions().DoubleWidth(1));
  }
  void update(int updateCount)
  { 
  }
  void collisionDetected(Sprite * spriteA, Sprite * spriteB, Point collisionPoint)
  {
  }

};





void setup()
{
  Ps3.begin("24:6f:28:af:1c:66");
  Serial.begin(115200);
  pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), contadorPulsos_ISR, RISING);
  DisplayController.begin();
  DisplayController.setResolution(VGA_320x200_75Hz);

}

void loop()
{
  SpeedOmeter speedOmeter;
  speedOmeter.start();
  FinalScene finalScene;
  finalScene.start();

  delay(1000);
}