#include <LedControl.h>

LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLOCK, LOW, DRIVER_NR

const int VRx = A1,
          VRy = A0,
          buttonPin = A2;
boolean movedX = false, movedY = false;
int prevX, prevY, posX, posY; //Player position
int minThreashold = 400, maxThreashold = 550;
// if in min - maxThreashold joyMoved = false;
int buttonState = 1;
unsigned long int prevPressTime = 0, //for the joystick button
              pressInterval = 100;
long int prevObs = 0; //last time generated obstacle
long int prevFall = 0, fallSpeed = 200; //for moving the objects

//points location vector
struct catchPoints{
  unsigned long int blinkTime;
  int X;
  int Y;
  bool state;
}*Food;
int FoodLen = 0, FoodInterval = 0,FoodIndex = 0;
unsigned long int lastFood = 0;
float foodMultiplier;     //catching more food give you more energy
int  theScore, Distance, Energy;
bool ghostMode = 0; //after hitting a wall
int ghostStart = 0; //where the player hit the wall
bool playerBlinkState = true;  //while ghost, player will blink
unsigned long prevGhostBlink = 0;

enum Difficulty{easy, medium, hard} Diff;
bool gameIsOver = false;
int lives = 0;
bool MapPrinted = 0;  //Print the map for the fist time

byte startMap[9]={
  B00000000,
  B01111100,
  B01000100,
  B01111100,
  B00010000,
  B00010000,
  B00101000,
  B01000100,
  B00000000
};
byte Map[9]={
  B00000000,
  B01111100,
  B01000100,
  B01111100,
  B00010000,
  B00010000,
  B00101000,
  B01000100,
  B00000000
};

//initiet the game variables
void gameSetup(){
  posX = 7;
  posY = 7;
  prevX = posX;
  prevY = posY;
  //lifes
  lives = 5 - Diff;
  FoodLen = Diff+4;
  Food = new catchPoints[FoodLen];
  for(int i=0;i<FoodLen;i++){
    Food[i].X = -1;
    Food[i].Y = 9;
  }
  lastFood = millis();
  FoodInterval = 3000/FoodLen;
  foodMultiplier = 1;
  //score
  Distance = 0;
  theScore = 0;
  Energy = 50;
  //the map
  for(int i=0;i<9;i++){
    Map[i] = startMap[i];
  }
  fallSpeed = 100*(2+Diff);
  prevObs = millis();
  prevFall = millis();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin,INPUT_PULLUP);

  //the lcd display
  setupLcd();

  //matrix
  lc.shutdown(0,false); //first driver
  lc.setIntensity(0,1); // 0-15;
  lc.clearDisplay(0);
  gameSetup();

  Serial.begin(9600);
}

void gameOver(){
  //a little animation to clean the matrix
  for(int row = 0;row<8;row++){
    for(int col = 0;col<8;col++){
      lc.setLed(0,row,col,true);
      delay(20);
    }
  }
  for(int row = 0;row<8;row++){
      for(int col = 0;col<8;col++){
        lc.setLed(0,col,row,false);
        delay(20);
      }
    }
    gameIsOver = true; 
    theScore = write_score(theScore,Energy);
}

void movement(){
  //move the player on tha matrix
  int valX = analogRead(VRx);
  int valY = analogRead(VRy);

  int changeY = 0, changeX = 0;
  if (valY > maxThreashold)
  {
    if (movedY == false)
    {
      changeY = -1;
      movedY = true;
    }
  }
  else
  {
    if (valY < minThreashold)
    {
      if (movedY == false)
      {
        changeY = 1;
        movedY = true;
      }
    }
    else movedY = false;
  }

  if (valX < minThreashold)
  {
    if (movedX == false)
    {
      changeX = -1;
      movedX = true;
    }
  }
  else
  {
    if (valX > maxThreashold)
    {
      if (movedX == false)
      {
        changeX = 1;
        movedX = true;
      }
    }
    else movedX = false;
  }

  prevX = posX;
  prevY = posY;
  posX += changeX;
  posY += changeY;
  // for wall collision
  if (posY == 8) posY = 7;
  if (posY == -1) posY = 0;

  if (posX == 8) posX = 7;
  if (posX == -1) posX = 0;

  //display the dot
  if(changeX || changeY)  //turn of the old position
    lc.setLed(0,prevY,prevX,false);
  lc.setLed(0,posY,posX,playerBlinkState);
}

void showFood(){
  //show food on the matrix
  //food is blinking
  for(int i=0;i<FoodLen;i++){
    if(Food[i].Y<8){
       if(millis() - Food[i].blinkTime > 50){
        Food[i].state = !Food[i].state;
        Food[i].blinkTime = millis();
       }
      lc.setLed(0,Food[i].Y,Food[i].X,Food[i].state);
    }
  }
}

void printMap(){
  for(int i=1;i<9;i++){
     lc.setRow(0, i-1, Map[i]);
  }
}

void hunger(){
  int hungerCoef = Distance / 50;
  
  if(Distance % 3 == 0){
    if(Energy > (Diff + hungerCoef))
      Energy -= (Diff + hungerCoef);   //lose energy in time
    else{
      lives--;
      if(lives<1){
        gameOver();
        MapPrinted = 0; 
      }
      else{
        //ghost mode
        ghostMode = true;
        ghostStart = Distance;
        prevGhostBlink = millis();
        Energy = 50;
        Distance/=2;
      }
    }
  }
}

//generate obstacoles
void makeObstacle(){
  byte wall = 1;
  int l = rand() % 2 + 1; //the length of the wall
  for(int i = 0;i<l;i++){
    wall <<= 1;
    wall++;
  }
  wall = wall << rand()%8; //move it somewhere
  Map[0] = wall;
  Map[1] = wall;
}

void makeFood(){
  if(millis() - lastFood > FoodInterval){
    if(Food[FoodIndex].Y>7){
      Food[FoodIndex].X = rand()%8;
      Food[FoodIndex].Y = 0;
      Food[FoodIndex].blinkTime = millis();
      Food[FoodIndex].state = true;
    }
    if(Map[Food[FoodIndex].Y+1] & 1<<(7-Food[FoodIndex].X)){
      Food[FoodIndex].Y = 8;
  }
    FoodIndex = (FoodIndex + 1)%FoodLen;
    lastFood = millis();
  }
}
void fallIngFood(){
  for(int i=0;i<FoodLen;i++){
    Food[i].Y++;
  }
}
void fallIng(){
  //make the map move
  if(millis()- prevObs>2000){
    makeObstacle();
    prevObs = millis();
  }
  if(millis()-prevFall > (1000 - fallSpeed)){
    //make the obstacles move
    for(int i = 8; i>0;i--){
      Map[i] = Map[i-1];
    }
    Map[0] = B00000000;
    prevFall = millis();
    printMap();
    //make the food move
    fallIngFood();
    //keep traking the distance
    Distance++;
    //if ghost don't lose energy any more
    if(!ghostMode)
      hunger();
    //lose multiplier
    foodMultiplier -= 0.05;
    if(foodMultiplier < 1)
      foodMultiplier = 1.0;
  }
}

void colision(){
  if(Map[posY+1] & 1<<(7-posX)){
      lives--;
      if(lives<1){
        gameOver();
        MapPrinted = 0;  
      }
      else{
        //ghost mode
        ghostMode = true;
        ghostStart = Distance;
        prevGhostBlink = millis();
      }
  }
}

void catchFood(){
  for(int i=0;i<FoodLen;i++){
    if(Food[i].X == posX && Food[i].Y == posY){
      Food[i].Y = 8;
      //some score
      theScore += (1+2*Diff);
      Energy += (5-Diff)*foodMultiplier;
      if(Energy > 250){
        Energy = 250;
      foodMultiplier += (0.1 + Diff/10.0);
      }
    }
  }
}

bool Pause = false, stopTime = false; //only for development

void pauseGame(){
  if(checkButton()){
    Pause = false;
    prevObs = millis();
  }
}

//the actual game
void feed_Joe(){
  if(!Pause && checkButton())
    Pause = true;
  if(!MapPrinted){
    printMap();
    MapPrinted = 1;
  }
  if(!Pause){
    fallIng();
    makeFood();
    movement();
  }
  else{
    pauseGame();
  }
  if(!ghostMode){
    colision();
    catchFood();
    showFood();
  }
  else{
    //while ghost - blink
    if(millis()-prevGhostBlink > 50){
      playerBlinkState = !playerBlinkState;
      prevGhostBlink = millis();
    }
    //ghostMode is active a limited time
    if(Distance - ghostStart > 5 - Diff){
      ghostMode = false;
      playerBlinkState = true;
    }
  }
}

void runMenu();

void loop() {
  runMenu();
}
