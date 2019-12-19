#include <LiquidCrystal.h>
#include <EEPROM.h>
const int RS = 7,
          E = 6,
          D4 = 5,
          D5 = 4,
          D6 = 3,
          D7 = 2,
          LCDLed = 8,   //for lcd Led
          VO = 9;       //for LCD contrast
LiquidCrystal lcd(RS,E,D4,D5,D6,D7); //pins

//JoyStick
//const int VRx = A1,
//          VRy = A0,
//          button = A2;
//bool joyMoved = false;
//int minThreashold = 400, maxThreashold = 550;
//// if in min - maxThreashold joyMoved = false;
//int buttonState = 1;
//unsigned long int prevPressTime = 0, //for the joystick button
//              pressInterval = 100;
#define joyMoved movedY
extern const int VRx, VRy, buttonPin;
extern bool movedY;
extern int minThreashold, maxThreashold;
extern int buttonState;
extern unsigned long int prevPressTime, pressInterval;
extern bool gameIsOver;
extern enum Difficulty Diff;
extern int lives;
const char diffStr[3][10] ={
  "easy",
  "medium",
  "hard"
};

//for printing on display
//the cursor
unsigned long int displayInterval = 10, prevDisplayTime = 0;
char lcdLedState = HIGH;    /* the lcd led */

//for menu
const int noOfMainRows = 4;
//int topMainRow = 0;   //the index for the row on the lcd main menu
char mainMenuRows[noOfMainRows][16]={
  "Start Game",
  "Score",
  "Settings",
  "Info"
};

//int subMenuRow = 0;     //for submenu rows like info
const int noOfInfo = 4;
char infoRows[noOfInfo][16]={
  "Feed Joe",
  "Cirstea Gabriel",
  "GitHub",
  "@UnibucRobotics"
};

char settingsRows[4][16]={
  "Level",
  "Light",
  "Contrast",
  "Back"
};
struct CurrentMenu{
  //if there will be a cursor
  bool cursorCheck;
  int cursorPos,prevCursorPos;
  int topRow;
  int noOfRows;
  char **theRows;
};
CurrentMenu *mainMenu, *infoMenu, *settingsMenu;

void initietMenus(){
  //o metoda nu prea ortodoxa
  mainMenu = new CurrentMenu;
  mainMenu->topRow = 0;
  mainMenu->noOfRows = 4;
  mainMenu->theRows = new char*[mainMenu->noOfRows];
  for(int i=0;i<mainMenu->noOfRows;i++){
    mainMenu->theRows[i] = mainMenuRows[i];
  }
  //if the cursor must be active
  mainMenu->cursorCheck = true;
  mainMenu->cursorPos = 0;
  mainMenu->prevCursorPos = 0;

  infoMenu = new CurrentMenu;
  infoMenu->topRow = 0;
  infoMenu->noOfRows = 4;
  infoMenu->theRows = new char*[infoMenu->noOfRows];
  for(int i=0;i<mainMenu->noOfRows;i++){
    infoMenu->theRows[i] = infoRows[i];
  }
  infoMenu->cursorCheck = false;

  settingsMenu = new CurrentMenu;
  settingsMenu->topRow = 0;
  settingsMenu->noOfRows = 4;
  settingsMenu->theRows = new char*[settingsMenu->noOfRows];
  for(int i=0;i<settingsMenu->noOfRows;i++){
    settingsMenu->theRows[i] = settingsRows[i];
  }
  settingsMenu->cursorCheck = true;
  settingsMenu->cursorPos = 0;
  settingsMenu->prevCursorPos = 0;
}

enum States{Menu,Game,Score,Settings,Info};
States MenuState = Menu;
enum SettingsStates{setMenu,SettingLevel,SettingLight,SettingContrast,Back};
SettingsStates SettingState = setMenu;


//game variables
//extern int theScore;
//extern int Distance;
int startingLevelValue = 0, Level = 0;
int HighScore;
int scoreAdr = 0; //address of the score in the EEPROM 
unsigned long int startTime, playingTime = 10000, lastLevIncr = 0;
//for lcd.clear() and print just once
int inited = 0;

void printCursor(char c,CurrentMenu *menu);

void printRows(struct CurrentMenu *menu){
  //print Rows on the lcd
  lcd.setCursor(1,0);
  lcd.print(menu->theRows[menu->topRow]);
  lcd.setCursor(1,1);
  lcd.print(menu->theRows[menu->topRow+1]);
}

void setupLcd() {
  // set the LCD first time
  lcd.begin(16,2); //colloms and rows
  lcd.clear();
  lcd.setCursor(0,0);
  pinMode(VRx,INPUT);
  pinMode(VRy,INPUT);
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(VO,OUTPUT);
  pinMode(LCDLed,OUTPUT);
  Serial.begin(9600);
  digitalWrite(LCDLed,lcdLedState);
  //analogWrite(VO,200);
  analogWrite(VO,110);

  initietMenus();
  printRows(mainMenu);
  printCursor('>',mainMenu);
  //load the highScore
  HighScore = EEPROM.read(scoreAdr);
}

void update_topRow(CurrentMenu *menu, int change){
  //the row on the top side
  menu->topRow += change;
  if(menu->topRow < 0)
    menu->topRow = 0;
  if(menu->topRow > (menu->noOfRows-2))
    menu->topRow = menu->noOfRows-2;
  lcd.clear();
  printRows(menu);
}

void printCursor(char c,CurrentMenu *menu){
  //the cursor indicate the selected row
  lcd.setCursor(0,menu->prevCursorPos);
  lcd.print(" ");
  lcd.setCursor(0,menu->cursorPos);
  lcd.print(c);
}

void updateY(int val,CurrentMenu *menu){
  //selected row
  menu->prevCursorPos = menu->cursorPos;
  menu->cursorPos -= val;
  if(menu->cursorPos<0){
        menu->cursorPos = 0;
        update_topRow(menu,-1);
      }
  if(menu->cursorPos>1){
        menu->cursorPos = 1;
        update_topRow(menu,1);
      }
  if(val)
    printCursor('>',menu);
}

int checkTheY(){
  int y = analogRead(VRy);
  if(y > maxThreashold){
    if(!joyMoved){
      //lcdYold = lcdY;
      //lcdY--;
      joyMoved = true;
      return -1;
    }
      joyMoved = true;
  }
  if(y < minThreashold){
    if(!joyMoved){
      //lcdYold = lcdY;
      //lcdY++;
      joyMoved = true;
      return 1;
    }
      joyMoved = true;
  }
  if(y>minThreashold && y<maxThreashold){
    joyMoved = false;
  }
  
  return 0;
}

int checkButton(){
  //if the button was press return 1
  buttonState = digitalRead(buttonPin);
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
//the menu
  updateY(checkTheY(),mainMenu);
  if(checkButton()){
    MenuState = mainMenu->topRow + mainMenu->cursorPos + 1;
    lcd.clear();
  }
//  if(millis() - prevDisplayTime > displayInterval){
//    printCursor('>');
//    //printRows();
//    prevDisplayTime = millis();
//  }
}

//for game
void game_init(){
  lives = 3;
  theScore = 0;
  Level = startingLevelValue;
  startTime = millis();
  lastLevIncr = millis();
  inited = 1;
  gameSetup();  //game setup for matrix
  lcd.clear();
}

unsigned write_score(int score, int bonusPoints){
  //calculate and set the final score
  int finalScore = score + bonusPoints;
  if(finalScore > HighScore){
    HighScore = finalScore;
    //save the score
    EEPROM.write(scoreAdr, HighScore);
  }
  return finalScore;
}

void end_game(){
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("___Well Done!___");
    lcd.setCursor(0,1);
    lcd.print("Score=");
    lcd.print(theScore);
    lcd.print("    ");
    if(checkButton()){
      MenuState = Menu;
      inited = 0;     /* to clear the screen */
      lcd.clear();
      printRows(mainMenu);
      printCursor('>',mainMenu);
      gameIsOver = false;
    }
}

//the game feed_Joe;
void feed_Joe();

void the_game(){
  //end of the game
  
  //theScore = 3*Level;
  lcd.setCursor(0,0);
  lcd.print("lives=");
  lcd.print(lives);
  lcd.print(" Score=");
  
  lcd.print(theScore);
  lcd.setCursor(0,1);
  lcd.print("Energy=");
  lcd.print(Energy);
  feed_Joe();     //run the game
}

void set_level(){
  /* set the difficulty */
  lcd.print("Set level & press");
  startingLevelValue += checkTheY();
  if(startingLevelValue < 0)
    startingLevelValue = 2;
  startingLevelValue %= 3;
  Diff = startingLevelValue;
  lcd.setCursor(0,1);
  lcd.print("Level: ");
  lcd.print(diffStr[startingLevelValue]);
  lcd.print("   ");
  if(checkButton()){
    //go back to menu
    lcd.clear();
    printRows(settingsMenu);
    SettingState = setMenu;
    inited = 0;   /* to clear the screen after this */
    printCursor('>',settingsMenu);
  }
}

void the_Settings(){
  /*display the settings */
  printRows(settingsMenu);
  updateY(checkTheY(),settingsMenu);
  if(checkButton()){
    SettingState = settingsMenu->topRow + settingsMenu->cursorPos + 1;
    lcd.clear();
  }
  
//  lcd.setCursor(0,0);
//  lcd.print("Set level & press");
//  set_level();
//  if(checkButton()){
//    //go back to menu
//    lcd.clear();
//    printRows(mainMenu);
//    MenuState = Menu;
//    inited = 0;
//    printCursor('>');
//  }
  
}

void print_highScore(){
  //display the highest score on LCD
  lcd.setCursor(0,0);
  lcd.print("The BEST:");
  HighScore = EEPROM.read(scoreAdr);
  lcd.print(HighScore);
  lcd.setCursor(0,1);
  lcd.print("back - press");
  if(checkButton()){
    inited = 0;
    lcd.clear();
    printRows(mainMenu);
    MenuState = Menu;
    printCursor('>',mainMenu);
  }
}

void show_info(){
  printRows(infoMenu);
  int change = checkTheY();
  if(change){
    update_topRow(infoMenu,-1*change);
  }
}

//void loop() {
void runMenu(){
  // main function for menu
  switch(MenuState){
    case Menu:
        menuNavigation();
        break;
    case Game:
      {
        if(!inited)
          game_init();  //init for Lcd
        if(!gameIsOver)
          the_game();
        else
          {
            end_game();
          }
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
        switch(SettingState){
          case setMenu:
          {   //display the settings
            the_Settings();
            break;
          }
          case SettingLevel:
          {   //set the difficulti level
            set_level();
            break;
          }
          case SettingLight:
          {
            lcdLedState = !lcdLedState;
            digitalWrite(LCDLed,lcdLedState);
            SettingState = setMenu;
            break;
          }
          case Back:
          {
            lcd.clear();
            printRows(mainMenu);
            MenuState = Menu;   //go back to main menu
            inited = 0;
            printCursor('>',mainMenu);
            SettingState = setMenu;   //come back on the settings later
            break;
          }
          default:
            SettingState = setMenu; //stai on the settings
            break;
        }
      }
      break;
      case Info:
      {
        show_info();
        if(checkButton()){
          MenuState = Menu;
          lcd.clear();
          printRows(mainMenu);
          printCursor('>',mainMenu);
        }
        break;
      }
      break;
  }
}
