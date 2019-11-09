const int redPin = 9, bluePin = 3, greenPin = 5, //output pins
          potPinRed = A0, potPinGreen = A1, potPinBlue = A2; //input pins
int redValueIn, greenValueIn, blueValueIn, //input values
    redValueOut, greenValueOut, blueValueOut; //output values

void setup() {
  // put your setup code here, to run once:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(potPinRed,INPUT);
  pinMode(potPinGreen,INPUT);
  pinMode(potPinBlue,INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //citim valorile de la potentiometre
  redValueIn = analogRead(potPinRed);
  greenValueIn = analogRead(potPinGreen);
  blueValueIn = analogRead(potPinBlue);

  //mapam pt valorile de output
  redValueOut = map(redValueIn, 0, 1023, 0, 255);
  greenValueOut = map(greenValueIn, 0, 1023, 0, 255);
  blueValueOut = map(blueValueIn, 0, 1023, 0, 255);

  //output
  analogWrite(redPin, redValueOut);
  analogWrite(greenPin, greenValueOut);
  analogWrite(bluePin, blueValueOut);
}
