#include "pitches.h"

const int buzzIn = A0, //passive buzzer for knock detecting
          buzzOut = 8, //active buzzer for the melody
          buttonPin = 2;
const int THRESHOLD = 300, interval = 100,
                            waitInterval = 5000; //interval for the knock
bool wait = 0;
unsigned long long int prevTimeButton = 0,
                       prevTimeBuzz = 0,
                       prevNoteTime = 0; //to replace the delay() in play_song
int buttonState;
// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzIn,INPUT);
  pinMode(buzzOut,OUTPUT);
  pinMode(buttonPin,INPUT);
  Serial.begin(9600);
}

int thisNote = 8;
void play_song(){
  if(thisNote<8){
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.20;
    if(millis() - prevNoteTime < pauseBetweenNotes){
      //noTone(buzzOut);
      return;
    }
    noTone(buzzOut);
    tone(buzzOut, melody[thisNote], noteDuration);

    
    //delay(pauseBetweenNotes);
    
    thisNote++;   //go to neste Note 
    prevNoteTime = millis();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  //check the knoks
  int knock = analogRead(buzzIn);
  if(knock > THRESHOLD){
   prevTimeBuzz = millis();
   wait = 1; //now waith some seconds
  }
  if(millis() - prevTimeBuzz > waitInterval){
    if(thisNote > 7 && wait)
      thisNote = 0; //start the melody;
      wait = 0;
  }

  //the button can start and stop the melody
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW){
    if(millis() - prevTimeButton > interval){
      if(thisNote < 8)  //melody is playing
        thisNote = 8; //stop the melody
      else
        thisNote = 0;  //start the melody
    }
    prevTimeButton = millis();
  }
  play_song();

  
}
