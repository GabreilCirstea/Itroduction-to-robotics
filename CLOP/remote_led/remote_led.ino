#include <IRremote.h>
#include "animations.h"

#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255
#define animINTERV 1000   //an animation interval

const int receiverPin = 11; //remote reciver
const int tranPin = 10; //transistor pin
const int redPin = 9, greenPin = 6, bluePin = 5;
struct RGBColor{
  int r,g,b;
}LastColor, CurrentColor; // last color befor the turn off
unsigned int brightness = 1;  //not used
//turn off and on
bool Running = 1;

IRrecv irrecv(receiverPin);
decode_results results;

void setColor(int red,int green, int blue);

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);

  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);

  pinMode(tranPin,OUTPUT);
  analogWrite(tranPin,200);
  analogWrite(3,0);
  
  LastColor = {0,0,0};
  CurrentColor = {0,0,0};
  
  //turn leds off
  setColor(0,0,0);
  Running = 0;
}
int applyBrightness(int color){
  //not working
  color += brightness*color/100;
  if(color > 255)
    color = 255;
  if(color < 0)
    color = 0;
  return color;
}

void setColor(int red, int green, int blue){
  //output the color
  if(!Running) return;  //if tuned off - exit
  CurrentColor = {red,green,blue};
  red = 255-red;
  green = 255-green;
  blue = 255-blue;
  
  red = applyBrightness(red);
  green = applyBrightness(green);
  blue = applyBrightness(blue);
  
  analogWrite(redPin,red);
  analogWrite(greenPin,green);
  analogWrite(bluePin,blue);
}
void clrcpy(RGBColor src, RGBColor &dst){
  //coppy color from src to dst
  dst.r = src.r;
  dst.g = src.g;
  dst.b = src.b;
}

void chekRmote(){
  if (irrecv.decode(&results)){
  Serial.println(results.value, HEX);
  switch(results.value){
    case 0xF700FF:{
      Serial.println("UP");
      brightness=(brightness-25)%100;
      setColor(CurrentColor.r,CurrentColor.g,CurrentColor.b);
    }
      break;
    case 0xF7807F:{
      Serial.println("DOWN");
      brightness=(brightness+25)%100;
      setColor(CurrentColor.r,CurrentColor.g,CurrentColor.b);
    }
      break;
    case 0xF740BF:{
      Serial.println("OFF");
      if(Running){
        clrcpy(CurrentColor,LastColor);
        setColor(0,0,0);
        Running = false;
      }
    }
      break;
    case 0xF7C03F:{
      Serial.println("ON");
      if(!Running){
        Running = true;
        if(!continueAnimation())
          setColor(LastColor.r,LastColor.g,LastColor.b);
      }
    }
      break;
    case 0xF720DF:{
      Serial.println("Red");
      stopAnimation();
      setColor(RED);
    }
    break;
    case 0xF7A05F:{
    Serial.println("Green");
    stopAnimation();
    setColor(GREEN);
    }
    break;
    case 0xF7609F:{
    Serial.println("Bleu");
    stopAnimation();
    setColor(BLUE);
    }
    break;
    case 0xF7E01F:{
    Serial.println("White");
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
    Serial.println("FLASH");
    initFlashAnim(animINTERV);
    }
    break;
    case 0xF7F00F:{
    Serial.println("STROBE");
    initStrobeAnim();
    }
    break;
    case 0xF7C837:{
    Serial.println("FADE");
    initFadeAnim(animINTERV);
    }
    break;
    case 0xF7E817:{
    Serial.println("SMOOTH");
    initSmoothAnim(animINTERV);
    }
    break;
    
    default:
    Serial.println("?");
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
