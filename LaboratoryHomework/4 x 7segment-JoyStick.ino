const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfDigits = 10;

int dpState = LOW;

int currentNumbers[noOfDisplays] = {1,0,0,0};
unsigned long delayCounting = 50;    // incrementing interval
unsigned long lastIncreasing = 0;
int currentPosition = 0; //the position of the cursor
int positionSet = 0;
// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
 };   
 
byte digitMatrix[noOfDigits][segSize - 1] = {
// a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

const int VRx = A1,
          VRy = A0,
          button = A2;
bool joyMoved = false;
int minThreashold = 400, maxThreashold = 550;
// if in min - maxThreashold joyMoved = false;
int buttonState = 1;
unsigned int prevTime = 0, //for the joystick button
              pressInterval = 100;

void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }

  // write the decimalPoint to DP pin
  digitalWrite(segments[segSize - 1], decimalPoint);
}

// activate the display no. received as param
void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW);
}
            
void showNumbers(){
  for(int i = 0; i <noOfDisplays;i++){
    delay(5);
    showDigit(i);
    if( positionSet && i == currentPosition)
      dpState = HIGH;
    else
      dpState = LOW;
    displayNumber(currentNumbers[i],dpState);
  }
}
void setup() {
  //JoyStick
  pinMode(VRx,INPUT);
  pinMode(VRy,INPUT);
  pinMode(button,INPUT_PULLUP);
  //4 x 7 segment display
  for (int i = 0; i < segSize; i++)
  {
    pinMode(segments[i], OUTPUT);  
  }
  for (int i = 0; i < noOfDisplays; i++)
  {
    pinMode(digits[i], OUTPUT);  
  }
  Serial.begin(9600);
}
void checkTheX(int x){
  if(x > maxThreashold){
    if(!joyMoved)
      currentPosition = (currentPosition + 1) % noOfDisplays;
      joyMoved = true;
  }
  if(x < minThreashold){
    if(!joyMoved)
      currentPosition--;
      if(currentPosition<0)
        currentPosition = noOfDisplays - 1;
      joyMoved = true;
  }
  if(x>minThreashold && x<maxThreashold){
    joyMoved = false;
  }
}
void checkTheY(int y){
  if(y > maxThreashold){
    if(!joyMoved)
      currentNumbers[currentPosition] =
                (currentNumbers[currentPosition]+1) % 10;
      joyMoved = true;
  }
  if(y < minThreashold){
    if(!joyMoved)
      currentNumbers[currentPosition]--;
      if(currentNumbers[currentPosition]<0)
        currentNumbers[currentPosition] = 9;
      joyMoved = true;
  }
  if(y>minThreashold && y<maxThreashold){
    joyMoved = false;
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  int x = analogRead(VRx);
  int y = analogRead(VRy);
  buttonState = digitalRead(button);
  if(!buttonState){
    if(millis() - prevTime > pressInterval ){
      positionSet = !positionSet;
    }
    prevTime = millis();
  }
  //move the cursor
  if(!positionSet)
    checkTheX(x);
  else
  //or change the number value
    checkTheY(y);
  showNumbers();
  
}
