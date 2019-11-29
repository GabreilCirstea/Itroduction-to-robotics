#include <LiquidCrystal.h>
const int RS = 12,
          E = 11,
          D4 = 5,
          D5 = 4,
          D6 = 3,
          D7 = 2;
LiquidCrystal lcd(RS,E,D4,D5,D6,D7); //pins

//JoyStick
const int VRx = A1,
          VRy = A0,
          button = A2;
bool joyMoved = false;
int minThreashold = 400, maxThreashold = 550;
// if in min - maxThreashold joyMoved = false;
int buttonState = 1;
unsigned long int prevPressTime = 0, //for the joystick button
              pressInterval = 100;

//for printing on display
unsigned long int displayInterval = 0, prevDisplayTime = 0;
int lcdYold = 0, lcdY = 0;
//for menu
const int noOfRows = 3;
int topRow = 0;   //the index for the row on the matrix
const char rows[noOfRows][16]={
  "Start Game",
  "Score",
  "Settings"
};

enum States{Menu,Game,Score,Settings};
int MenuState = Menu;

void printRows(){
  lcd.setCursor(1,0);
  lcd.print(rows[topRow]);
  lcd.setCursor(1,1);
  lcd.print(rows[topRow+1]);
}

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2); //colloms and rows
  lcd.clear();
  lcd.setCursor(0,0);
  pinMode(VRx,INPUT);
  pinMode(VRy,INPUT);
  pinMode(button,INPUT_PULLUP);
  printRows();
  Serial.begin(9600);
}

void update_topRow(int change){
  topRow += change;
  if(topRow < 0)
    topRow = 0;
  if(topRow > (noOfRows-2))
    topRow = noOfRows-2;
  lcd.clear();
  printRows();
}

void updateY(int val){
  lcdYold = lcdY;
  lcdY -= val;
  if(lcdY<0){
        lcdY = 0;
        update_topRow(-1);
      }
  if(lcdY>1){
        lcdY = 1;
        update_topRow(1);
      }
}

int checkTheY(){
  int y = analogRead(VRy);
  if(y > maxThreashold){
    if(!joyMoved){
      //lcdYold = lcdY;
      //lcdY--;
      joyMoved = true;
      return 1;
    }
      joyMoved = true;
  }
  if(y < minThreashold){
    if(!joyMoved){
      //lcdYold = lcdY;
      //lcdY++;
      joyMoved = true;
      return -1;
    }
      joyMoved = true;
  }
  if(y>minThreashold && y<maxThreashold){
    joyMoved = false;
  }
  
  return 0;
}

void printCursor(char c){
  lcd.setCursor(0,lcdYold);
  lcd.print(" ");
  lcd.setCursor(0,lcdY);
  lcd.print(c);
}

int checkButton(){
  //if the button was press return 1
  buttonState = digitalRead(button);
  if(!buttonState){
    if(millis() - prevPressTime > pressInterval){
      prevPressTime = millis();
      return 1;
    }
    prevPressTime = millis();
  }
  return 0;
}

void menuNavigation(){
  //checkTheX(x);
  updateY(checkTheY());
  if(checkButton())
    MenuState = topRow + lcdY + 1;
  if(millis() - prevDisplayTime > displayInterval){
    printCursor('>');
    //printRows();
    prevDisplayTime = millis();
  }
}
//for game
int lives = 3, startingLevelValue = 0, Level = 0,
    theScore = 0, HighScore = 0;
unsigned long int startTime, playingTime = 10000, lastLevIncr = 0;
int inited = 0;
void game_init(){
  lives = 3;
  theScore = 0;
  Level = startingLevelValue;
  startTime = millis();
  lastLevIncr = millis();
  inited = 1;
  lcd.clear();
}

void end_game(){
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("___Well Done!___");
    lcd.setCursor(0,1);
    lcd.print("__Press button__");
    if(checkButton()){
      MenuState = Menu;
      inited = 0;
      lcd.clear();
      printRows();
    }
}
void the_game(){
  //end of the game
  if(theScore > HighScore)
    HighScore = theScore;
  if(millis() - startTime > playingTime){
    end_game();
    return;
  }
  theScore = 3*Level;
  lcd.setCursor(0,0);
  lcd.print("lives=");
  lcd.print(lives);
  lcd.print(" Score=");
  if((millis() - lastLevIncr)>5000){
    Level++;
    lastLevIncr = millis();
  }
  lcd.print(theScore);
  lcd.setCursor(0,1);
  lcd.print("Level=");
  lcd.print(Level);

}

void set_score(){
  startingLevelValue += checkTheY();
  if(startingLevelValue < 0)
    startingLevelValue = 5;
  startingLevelValue %= 6;
  lcd.setCursor(0,1);
  lcd.print("Level=");
  lcd.print(startingLevelValue);
}

void the_Settings(){
  lcd.setCursor(0,0);
  lcd.print("Set level & press");
  set_score();
  if(checkButton()){
    //go back to menu
    lcd.clear();
    printRows();
    MenuState = Menu;
    inited = 0;
  }
  
}

void print_highScore(){
  lcd.setCursor(0,0);
  lcd.print("The BEST:");
  lcd.print(HighScore);
  lcd.setCursor(0,1);
  lcd.print("back - press");
  if(checkButton()){
    inited = 0;
    lcd.clear();
    printRows();
    MenuState = Menu;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //int x = analogRead(VRx);
  switch(MenuState){
    case Menu:
        menuNavigation();
        break;
    case Game:
      {
        if(!inited)
          game_init();
        the_game();
      }
      break;
     case Score:
     {
      if(!inited){
        lcd.clear();
        inited = 1;
      }
      print_highScore();
     }
      break;
     case Settings:
      {
        if(!inited){
          //fist time clear the screen
          lcd.clear();
          inited = 1;
        }
        the_Settings();
      }
      break;
  }
  Serial.print("stadiu:");
  Serial.println(MenuState);
}
