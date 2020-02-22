#include <IRremote.h>
#include <FastLED.h>
#include "animations.h"

#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255
#define animINTERV 1000   //an animation interval
#define MAX_BRIGHT 90     //maximum brighness
//number of pins
#define LED_NUM 26        //the number of leds per strip
#define LED_STRIPS 2      //the number of leds strips
#define LED_BOTTOM 10
#define LED_LEFT 9

//the leds strips as a matrix
CRGB leds[LED_STRIPS][LED_NUM];

const int receiverPin = 11;
struct RGBColor{
  int r,g,b;
}LastColor, CurrentColor; // last color befor the turn off
unsigned int brightness = 5;  //the brightness
//turn off and on the LEDs
bool Running = 1;

IRrecv irrecv(receiverPin);
decode_results results;

void setColor(int red,int green, int blue);

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);     //when receveing signal, blink build in led

  //init the LEDs
  FastLED.addLeds<WS2812,LED_BOTTOM,GRB>(leds[0],LED_NUM);
  FastLED.addLeds<WS2812,LED_LEFT,GRB>(leds[1],LED_NUM);
  FastLED.setBrightness(brightness);
  
  LastColor = {0,0,0};
  CurrentColor = {0,0,0};
  
  //turn leds off
  Running = 0;
}
void applyBrightness(unsigned int bright){
  //change the brightness of the LEDs
  FastLED.setBrightness(bright);
  FastLED.show();
}

void setColor(int red, int green, int blue){
  //output the color
  if(!Running) return;      //if tuned off -> exit
  CurrentColor = {red,green,blue};
  for(int j=0;j<LED_STRIPS;j++)
    for(int i=0;i<LED_NUM;i++){
      leds[j][i] = CRGB(red,green,blue);
   }
 FastLED.show();
}
void clrcpy(RGBColor src, RGBColor &dst){
  //coppy color from src to dst
  dst.r = src.r;
  dst.g = src.g;
  dst.b = src.b;
}
// a special animation
#define FSTEP 10
void fadeTo(CRGB src, CRGB& dst){
  //fade from src to dst
  bool ok = true;
  for(int i=0;i<3;i++){
   int diff = src[i] - dst[i];
    if(diff < FSTEP){
      dst[i] += FSTEP;
    }
    else if(diff > FSTEP){
      dst[i] -= FSTEP;
    }
  }
}
void initLedStrip(){      //spread the colors to the LED strip
  RunningNow = SMOOTH;
  int indexColor = 0;
  for(int j=0;j<LED_STRIPS;j++)
  for(int i=0;i<LED_NUM;i++){
    if(i%(LED_NUM/animColorsNo) == 0){
      leds[j][i] = CRGB(animColors[i][0],animColors[i][1],animColors[i][2]);
      indexColor = 0;
    }
    else{
      fadeTo(leds[j][i-1],leds[j][i]);
    }
  }
  FastLED.show();
}
void moveAround(){
  //move leds around
  EVERY_N_MILLISECONDS(100){
    CRGB aux = leds[0][0];
    for(int j=0;j<LED_STRIPS;j++){
      
      for(int i=0;i<LED_NUM;i++){
        leds[j+(i/(LED_NUM))][i] = leds[j][(i+1)%LED_NUM];
        //fadeTo(leds[(i+1)%NUM],leds[i]);
      }
      
      //fadeTo(aux,leds[NUM-1]);
      FastLED.show();
    }//for j
    leds[LED_STRIPS-1][LED_NUM-1] = aux;
  }
} //change

void chekRmote(){
  if (irrecv.decode(&results)){
  Serial.println(results.value, HEX);
  switch(results.value){
    case 0xF700FF:{
      //Serial.print("UP ");
      //Serial.println(brightness);
      brightness+=10;
      if(brightness > MAX_BRIGHT)
        brightness = MAX_BRIGHT;
      applyBrightness(brightness);
    }
      break;
    case 0xF7807F:{
      //Serial.println("DOWN");
      brightness-=10;
      if(brightness < 0)
        brightness = 0;
      applyBrightness(brightness);
    }
      break;
    case 0xF740BF:{
      //Serial.println("OFF");
      if(Running){
        clrcpy(CurrentColor,LastColor);
        setColor(0,0,0);
        Running = false;
      }
    }
      break;
    case 0xF7C03F:{
      //Serial.println("ON");
      if(!Running){
        Running = true;
        if(!continueAnimation())
          setColor(LastColor.r,LastColor.g,LastColor.b);
      }
    }
      break;
    case 0xF720DF:{
      //Serial.println("Red");
      stopAnimation();
      setColor(RED);
    }
    break;
    case 0xF7A05F:{
    //Serial.println("Green");
    stopAnimation();
    setColor(GREEN);
    }
    break;
    case 0xF7609F:{
    //Serial.println("Bleu");
    stopAnimation();
    setColor(BLUE);
    }
    break;
    case 0xF7E01F:{
    //Serial.println("White");
    stopAnimation();
    setColor(255,255,255);
    }
    break;
    //the red collom
    case 0xF710EF:{
      stopAnimation();
      setColor(200, 64, 0);
    }
    break;
    case 0xF730CF:{
      setColor(255, 128, 0);
    }
    break;
    case 0xF708F7:{
      stopAnimation();
      setColor(255, 191, 0);
    }
    break;
    case 0xF728D7:{
      stopAnimation();
      setColor(255, 255, 0);
    }
    break;
    //the green collom
    case 0xF7906F:{
      stopAnimation();
      setColor(0, 255, 64);
    }
    break;
    case 0xF7B04F:{
      stopAnimation();
      setColor(0, 255, 128);
    }
    break;
    case 0xF78877:{
      stopAnimation();
      setColor(0, 255, 191);
    }
    break;
    case 0xF7A857:{
      stopAnimation();
      setColor(0, 64, 255);
    }
    break;
    //the blue collom
    case 0xF750AF:{
      stopAnimation();
      setColor(64, 0, 255);
    }
    break;
    case 0xF7708F:{
      stopAnimation();
      setColor(128, 0, 255);
    }
    break;
    case 0xF748B7:{
      stopAnimation();
      setColor(191, 0, 255);
    }
    break;
    case 0xF76897:{
      stopAnimation();
      setColor(255, 0, 255);
    }
    break;
    
    case 0xF7D02F:{
    //Serial.println("FLASH");
    initFlashAnim(animINTERV);
    }
    break;
    case 0xF7F00F:{
    //Serial.println("STROBE");
    initStrobeAnim();
    }
    break;
    case 0xF7C837:{
    //Serial.println("FADE");
    initFadeAnim(animINTERV);
    }
    break;
    case 0xF7E817:{
    //Serial.println("SMOOTH");
    //initSmoothAnim(animINTERV);
    initLedStrip();
    }
    break;
    
    default:
    //Serial.println("?");
    break;
  }
  irrecv.resume();
  }
}

void loop(){
  chekRmote();
  if(Running){
    continueAnimation();
  }
}
