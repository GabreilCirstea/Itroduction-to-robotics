#define COLOR_CODE 3
#define FLASH_BLINK 4
#define SMOOTH_LENGHT 3 //smooth is a flash, with just first colors

void setColor(int red, int green, int blue);
//double lastTimeAnim = 0, intervAnim;
//int animIndex = 0;
const int animColorsNo = 6;
int animColors[animColorsNo][COLOR_CODE]={
  {255,0,0},  //red
  {0,255,0},  //green
  {0,0,255},  //blue
  {255, 128, 0},
  {0, 255, 64},
  {64, 0, 255}
};
//list of animations
enum animationPlaying{NOTHING,FLASH,STROBE,FADE,SMOOTH};
int RunningNow = NOTHING;   //animation running

struct AnimInfo{
  char *animationName;  //the name
  double lastTimeAnim, timeInterv;
  int animIndex;
  //when it needs
  int nowColor[COLOR_CODE];      //the current color in the animation
  double interStepTime;          //for fade and strobe
  int colorStep;                 //to change color
}animStep;

void setAnimColor(int colors[COLOR_CODE]){
  setColor(colors[0],colors[1],colors[2]);
}

void playFlash(AnimInfo& animStep){
  //timeInterv - the time between colors
  //jump from a color to another
  if(millis()-animStep.lastTimeAnim > animStep.timeInterv){
    animStep.animIndex=(animStep.animIndex+1)%animColorsNo;
//    setColor(animColors[animIndex][0],
//      animColors[animIndex][1],animColors[animIndex][2]);
    setAnimColor(animColors[animStep.animIndex]);
    animStep.lastTimeAnim = millis();
  }
  else{
    //a little blink with next color
    if(millis()-animStep.lastTimeAnim > animStep.interStepTime){
      if(millis()-animStep.lastTimeAnim < 
                animStep.timeInterv-animStep.interStepTime){
      //if it is between interStepTime and timeInter-internStepTime
      //a blink with the next color
                  int next = (animStep.animIndex+1)%animColorsNo;
                  setAnimColor(animColors[next]);
                }
      else{
        setAnimColor(animColors[animStep.animIndex]);
      }
    }
  }
}
void initFlashAnim(double timeInterv){
  RunningNow = FLASH;
  //set the structure
  //mai the name will be here too
  animStep.lastTimeAnim = 0;
  animStep.timeInterv = timeInterv;
  animStep.interStepTime = timeInterv/FLASH_BLINK;
  animStep.animIndex = 0;
  //set the first color
//  setColor(animColors[animIndex][0],
//      animColors[animIndex][1],animColors[animIndex][2]);
  setAnimColor(animColors[animStep.animIndex]);
}

void playStrobe(AnimInfo& animStep){
  //the color
  if(millis() - animStep.lastTimeAnim > animStep.timeInterv){
    for(int i=0;i<COLOR_CODE;i++){
      if(animStep.nowColor[i] < 0 || animStep.nowColor[i] > 250){
        animStep.colorStep *= -1;
        break;
      }
    }
    for(int i=0;i<3;i++){
      animStep.nowColor[i]+=animStep.colorStep;
    }
    setAnimColor(animStep.nowColor);
    animStep.lastTimeAnim = millis();
  }
}

void initStrobeAnim(){
  RunningNow = STROBE;
  //set the structure
  //may the name will be here too
  animStep.lastTimeAnim = 0;
  animStep.timeInterv = 30;   //time to modify the color
  animStep.animIndex = 0;
  animStep.colorStep = 5;
  for(int i=0;i<3;i++){
    animStep.nowColor[i] = 255;
  }
  setAnimColor(animStep.nowColor);
}

//int nowColor[3],fadeStep = 0;
//double lastFade = 0;
void playFade(AnimInfo& animStep){
  //in the timeInterv it has to get to the next color
  //have to fade to animColors[animIndex]
  if(millis() - animStep.lastTimeAnim > 30){ //a shor interv to anim
    bool change = false;
    for(int i=0;i<COLOR_CODE;i++){
      //the diffrence between values
      int diff = animStep.nowColor[i] - animColors[animStep.animIndex][i];
      if(diff < -3){
        animStep.nowColor[i] += 3;
        change = true;
      }
      else if(diff > 3){
        animStep.nowColor[i] -= 3;
        change = true;
      }
    } //for
    if(!change){
        animStep.animIndex = (animStep.animIndex + 1) % animColorsNo; 
      }
    setAnimColor(animStep.nowColor);
    animStep.lastTimeAnim = millis();
  } //if
} //function

void initFadeAnim(double timeInterv){
  RunningNow = FADE;
  //set the structure
  //may the name will be here too
  animStep.lastTimeAnim = 0;
  animStep.timeInterv = timeInterv;
  animStep.animIndex = 0;
  //setAnimColor(animColors[animStep.animIndex]);
  for(int i=0;i<COLOR_CODE;i++){
    animStep.nowColor[i] = 0;
  }
}

void playSmooth(AnimInfo& animStep){
  //a flash without blink, and only pure colors
  if(millis() - animStep.lastTimeAnim > animStep.timeInterv){
    animStep.animIndex=(animStep.animIndex+1)%SMOOTH_LENGHT;
    setAnimColor(animColors[animStep.animIndex]);
    animStep.lastTimeAnim = millis();
  }
}

void initSmoothAnim(double timeInterv){
  RunningNow = SMOOTH;
  //set the structure
  //may the name will be here too
  animStep.lastTimeAnim = 0;
  animStep.timeInterv = timeInterv;
  animStep.animIndex = 0;
  setAnimColor(animColors[animStep.animIndex]);
}

int continueAnimation(){
  switch(RunningNow){
    case FLASH:{
      //cum tin minte si intervalul?
      playFlash(animStep);
      //return 1;
    }
    break;
    case FADE:{
      playFade(animStep);
      //return 1;
    }
    break;
    case STROBE:{
      playStrobe(animStep);
      return 1;
    }
    break;
    case SMOOTH:{
      playSmooth(animStep);
      return 1;
    }
    break;
    default:
      return 0;
    
  }
}
void stopAnimation(){
  RunningNow = NOTHING;
}
