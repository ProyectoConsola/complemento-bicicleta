// #include<ESP32Time.h>
#include <Arduino.h>
#include "fabgl.h"
#include "fabutils.h"
#include <iostream>
#include <stdlib.h>
#include <Ps3Controller.h>
#define MAX_V  40

using fabgl::iclamp;
using std::string;

fabgl::VGAController DisplayController;
fabgl::Canvas canvas(&DisplayController);

  const int PUL = 25;
  const int DIR = 26;
  const int ENA = 27;
  int contador = 0;


const int pinEncoder = 12;
unsigned int cntPulsos;

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

/*
static constexpr uint8_t selection_arrow_bits[18] = {
        0x00, 0x40, 0x00, 0x60, 0x00, 0x70, 0x1f, 0xf8, 0x1f, 0xfc, 0x1f, 0xf8,
        0x00, 0x70, 0x00, 0x60, 0x00, 0x40
    };
Bitmap flecha_seleccion = Bitmap(16, 9, selection_arrow_bits, PixelFormat::Mask, RGB888(231, 158, 35));
Sprite arrow;
struct IntroScene : public Scene
{
  static const int TEXTROWS = 4;
  static const int TEXT_X   = 130;
  static const int TEXT_Y   = 122;
  
  int starting_ = 0;
  int selectedLevel = 0;
  uint8_t LEVEL_NUMBER = 5;
  int start_pos=35;


  IntroScene()
  : Scene(0, 20, DisplayController.getViewPortWidth(), DisplayController.getViewPortHeight())
  {
  }

  void init(){
    canvas.selectFont(&fabgl::FONT_8x13);
    canvas.setPenColor(255, 255, 255);
    canvas.setBrushColor(255, 0, 0);
    canvas.drawText(10, 10, "SELECCIONE UN NIVEL:");
    canvas.selectFont(&fabgl::FONT_8x9);
    canvas.drawText(16, 36, "Nivel 1");
    canvas.drawText(16, 66, "Nivel 2");
    canvas.drawText(16, 96, "Nivel 3");
    canvas.drawText(16, 126, "Nivel 4");
    canvas.drawText(16, 156, "Nivel 5");

    arrow.addBitmap(&flecha_seleccion);
    arrow.moveTo(1,35);
    arrow.visible=true;
    DisplayController.setSprites(&arrow, 1);
    
    
  }
  void update(int updateCount){

    if(starting_){
      if(starting_ > 50){
        stop();
      }
      ++starting_;
      canvas.setBrushColor(Color::Black);
      arrow.visible=false;
      canvas.scroll(0, -5);
    }

    currentTime = millis();
    if ((currentTime - lastInputRecieved) > INPUT_DELAY){
      lastInputRecieved = currentTime;
      if(Ps3.data.button.down){
        selectedLevel=(selectedLevel + 1) % LEVEL_NUMBER;
        arrow.y= start_pos + selectedLevel * 30;
      }
      else if(Ps3.data.button.up){
        selectedLevel = (selectedLevel != 0)? selectedLevel - 1: LEVEL_NUMBER - 1;
        arrow.y= start_pos + selectedLevel * 30;
      }
      else if(Ps3.data.button.cross){
        switch(selectedLevel){
          case 0:
          starting_=true;
              //Cantidad de pulsos
              break;
          case 1:
          starting_=true;
              break;
          case 2:
          starting_=true;
              break;
          case 3:
          starting_=true;
              break;
          case 4:
          starting_=true;
              break;
          default:
              break;
        }
        
      }
    }
    
    DisplayController.refreshSprites();
  }
  void collisionDetected(Sprite * spriteA, Sprite * spriteB, Point collisionPoint){

  }

};


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
      distanciaM = ((cntPulsos / 20.0) * 2 * PI * 0.32);
      distanciaT += distanciaM;
      Vmps = distanciaM / (cntTiempo / 1000.0) * 3.66667; // 3.6667 es para pasar los m/s a km/h
      
      snprintf(strDistancia, 20, "%.4g", distanciaT);
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
  void collisionDetected(Sprite *spriteA, Sprite *spriteB, Point collisionPoint)
  {
  }
};
*/




void setup()
{
  Ps3.begin("24:6f:28:af:1c:66");
  Serial.begin(115200);
  /*pinMode(pinEncoder, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinEncoder), contadorPulsos_ISR, RISING);
  DisplayController.begin();
  DisplayController.setResolution(VGA_320x200_75Hz);
  Serial.println("¡Listo!");*/

  //Calibrador de pulsos

    //Serial.begin(115200);
    pinMode(PUL, OUTPUT);
    pinMode(DIR,OUTPUT);
    pinMode(ENA,OUTPUT);
    
    digitalWrite(ENA,LOW);
  

}

void loop()
{
  /*IntroScene introScene;
  introScene.start();
  SpeedOmeter speedOmeter;
  speedOmeter.start();*/


 if (Ps3.data.button.cross) {
    digitalWrite(DIR,LOW);
    for(int i = 0; i < 5; i++){
        digitalWrite(PUL,HIGH);
        delay(16);
        digitalWrite(PUL, LOW);
        delay(16);
    }
  }
  if (Ps3.data.button.circle) {
    digitalWrite(DIR,HIGH);
    for(int i = 0; i < 5; i++){
        digitalWrite(PUL,HIGH);
        delay(16);
        digitalWrite(PUL, LOW);
        delay(16);
    }
  }
 

  
  
  //delay(1);
}