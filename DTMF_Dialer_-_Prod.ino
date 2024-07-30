  
#include <Arduino.h>
#include "ButtonMatrix.h" 
#include "AdafruitI2CIOHandler.h" /** This is required for the ButtonMatrix to work with the Adafruit i2c ioexpansion */
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;


//START RGB LIGHTS
//Pin Assignments
int LED_Count = 10;
int Anodes[10] = {24,23,4,3,5,6,7,8,9,10};
int Mode_Btn =  26;
int Blue     =  2;  //PWM Only
int Red      =  0;  //PWM Only
int Green    =  1;  //PWM Only
int RandomSeedPin = 26;

int PAM_CE   = 14; //Active High Amp Enable
int Blink    = 13;

int Mode = 0;
long timeout;
long my_timeout = 5000;  //Timeout for not pressing a button
int ModeButton = 0;
int Steps = 0;
int Animation = 0;
int r,g,b;

int reading_state = 0;     //should we be reading buttons right now?
int number_of_digits = 0;  //what digit are we on in the array?
String my_input = "";      //Create a string to hold our values
char my_input_array[8];  //Create a 5 digit array to store button presses

String Correct1 = "911";  //The correct button press combo
String Correct2 = "8675309";
String Correct2_3 = "867";
String Correct3 = "77733222";
String Correct3_3 = "777";

long start_millis;
long end_millis;

int max_digits = 3;

//**********************************************
//**********************************************
//**********************************************

//START KEYPAD CODE
//MCP23017 Pin Assignments

using namespace RSys;

const uint16_t longPressDuration = 1000; /** Minimum duration of a long press */
const uint8_t COLS = 3;
const uint8_t ROWS = 4;
uint8_t colPins[COLS] = {10, 6, 7};
uint8_t rowPins[ROWS] = { 5, 9, 8, 4};

/*
byte rows[] = {5,9,8,4};
byte cols[] = {10,6,7};
const int rowCount = sizeof(rows)/sizeof(rows[0]);
const int colCount = sizeof(cols)/sizeof(cols[0]);
byte keys[colCount][rowCount];
*/

RSys::Button buttons[ROWS][COLS] = {
    { (0), (1), (2) },
    { (3), (4), (5) },
    { (6), (7), (8) },
    { (9), (10), (11) }
};

ButtonMatrix matrix((Button*)buttons, rowPins, colPins, ROWS, COLS, ADFI2C(mcp));

//char keypress;

/*
for (i = 0; i > NoCols; i++) {
  mcp.digitalWrite(Cols[i], HIGH);
  row_1 = mcp.digitalRead(Rows[0])
  row_2 = mcp.digitalRead(Rows[1])
  row_3 = mcp.digitalRead(Rows[2])
  row_4 = mcp.digitalRead(Rows[3])
}
*/

//END KEYPAD CODE

//**********************************************
//**********************************************
//**********************************************

//START DTMF CODE
//MCP23017 Pin Assignments
int DTMF_Data_0 = 2;
int DTMF_Data_1 = 3;
int DTMF_Data_2 = 0;
int DTMF_Data_3 = 1;

int DTMF_OUT[4] = {0,0,0,0};

//LSB must be in the left position
int Bin[11][4] = {{1,0,0,0}, {0,1,0,0}, {1,1,0,0}, 
                  {0,0,1,0} ,{1,0,1,0}, {0,1,1,0}, 
                  {1,1,1,0}, {0,0,0,1}, {1,0,0,1}, 
                  {0,1,0,1}, {1,0,1,1}};

int HT9200_CE = 16; //ATMEGA328PB Pin

//*************************************************************
//*************************************************************
//*************************************************************

int white[]  = {255, 255, 255};
int red[]    = {255,   0,   0};
int cyan[]   = {255,  65,   0};
int orange[] = {180,  20,  20};
int yellow[] = {255,  65,   0};
int purple[] = { 20,   0, 255};
int green[]  = {  0, 255,   0};
int blue[]   = {  0,   0, 255};
int pink[]   = { 20,   0, 255};
int noColors = 45;



int color[][3] = {
    {255, 255, 255}, {255,   0,   0}, {255,  65,   0},
    {180,  20,  20}, {255,  65,   0}, { 20,   0, 255},
    {  0, 255,   0}, {  0,   0, 255}, { 20,   0, 255},
    {  0, 255, 255}, {255,   0, 255}, {255, 255,   0},
    {  0,   0, 255}, {255,   0,   0}, {  0, 255,   0}, 
    { 44,  44,  44}, { 86,  86,  86}, {  0,  90, 255}, 
    {  0,  63,  52}, {127, 255, 127}, { 90, 213, 213}, 
    { 82,  39,  25}, {255,   0, 255}, {  0,  40, 255}, 
    { 25,  25,   5}, {  0, 128, 175}, {127, 127, 255}, 
    {255, 127, 127}, {128, 128, 128}, {255, 105, 180},
    { 70, 130, 180}, {240, 230, 140}, {173, 216, 230},
    {255, 239, 213}, {220,  20,  60}, { 75,   0, 130},
    {255, 228, 181}, {100, 149, 237}, {218, 165,  32},
    {135, 206, 250}, {  0, 191, 255}, {154, 205,  50},
    {255,  69,   0}, {255, 218, 185}, {255, 250, 205}
};


void setup() {

  pinMode(Blue,  OUTPUT);
  pinMode(Red,   OUTPUT);
  pinMode(Green, OUTPUT);
  digitalWrite(Blue,  HIGH);
  digitalWrite(Red,   HIGH);
  digitalWrite(Green, HIGH);
  pinMode(Blink, OUTPUT);

  digitalWrite(Blink, HIGH);
  delay(100);
  digitalWrite(Blink, LOW);
  delay(100);
  
  pinMode(PAM_CE, OUTPUT);
  digitalWrite(PAM_CE, HIGH);

  randomSeed(analogRead(RandomSeedPin));

  
  for(int i = 0; i < LED_Count; i++) {
    pinMode(Anodes[i], OUTPUT);
    digitalWrite(Anodes[i], LOW);
  }

  pinMode(HT9200_CE, OUTPUT);
  digitalWrite(HT9200_CE, HIGH);  //DTMF Chip Active Low Enable
  
  delay(100);

  //Configure pinMode for MCP23017
  if (!mcp.begin_I2C(0x20)) {
    while(1){
      POST(50);
    }
  }
  
  matrix.init(); //Initailize Button Matrix
  matrix.setScanInterval(20);
  matrix.setMinLongPressDuration(longPressDuration); // Set the long press duration in ms


  mcp.pinMode(DTMF_Data_0, OUTPUT);
  mcp.pinMode(DTMF_Data_1, OUTPUT);
  mcp.pinMode(DTMF_Data_2, OUTPUT);
  mcp.pinMode(DTMF_Data_3, OUTPUT);

  pinMode(HT9200_CE, OUTPUT);
  reset();

  white_chase(10);
  white_chase(25);
  white_chase(50);

}


//*************************************************************
//*************************************************************
//*************************************************************

//int BTNC = 0;

void loop() {
  

  switch(Mode) {
    case 0:   //Animation
      switch(Animation) {
        case 0:


        if(ModeButton == 1) {
            reset_leds();
            Animation = 1;
          }
          break;
        
        case 2://rainbow setup
          r = 255;
          g = 255;
          b = 0;
          Animation = 10;
          Steps=0;

          analogWrite(Red, r); analogWrite(Green, g); analogWrite(Blue, b);

        case 10://Rainbow
          rainbow();
          Steps++;
          if(Steps > 5) { Steps == 0; }
          if(ModeButton == 1) {
            reset_leds();
            Animation = 0;
          }
          break;
        case 20://Not used
          //put function here
          if(ModeButton == 1) {
            reset_leds();
            Animation = 0;
          }
        case 1:  //White Chase
          white_chase(50);
          if(ModeButton == 1) {
            reset_leds();
            Animation = 2;
          } 
          break;
        case 3: //911 Mode
          Do911();
          if(ModeButton == 1) {
            reset_leds();
            Animation = 0;
          } 
          break;  
        case 4: // 8675309 Win Condition
          partyMode();
          if(ModeButton == 1) {
            reset_leds();
            Animation = 0;
          } 
          break;
        case 5: //Win Condition 3 (SEC)
          partyMode();
          if(ModeButton == 1) {
            reset_leds();
            Animation = 0;
          } 
          break;       
      }
      break;
    case 1:   //A button was pressed, so stay here while reading buttons.
      if(millis() > timeout) {
        ResetVars();
        for(int i = 0; i < LED_Count; i++) {
          digitalWrite(Anodes[i], HIGH);
        }

        digitalWrite(Blue, HIGH);
        digitalWrite(Green, HIGH);

        for(int i = 0; i < 255; i++) {
          analogWrite(Red, i);
          delay(5);
        }
        reset();
        Mode = 0;   //No button was pressed in a while, so go back to Mode 0 (Animation Mode)
        //Animation = 0;
      }
      break;      
  }
  ModeButton = 0; 
  CheckButtons();
}

void Do911() {
    for(int i = 0; i < LED_Count; i++) {   
        ModeButton = 0; 
        CheckButtons();
        if(ModeButton == 1) { break; }
        digitalWrite(Anodes[i], HIGH);
    }   
    analogWrite(Blue, 0);
    analogWrite(Green, 255);
    analogWrite(Red, 255);
    delay(500);
    CheckButtons();
    analogWrite(Blue, 255);
    analogWrite(Green, 255);
    analogWrite(Red, 0);
    delay(500);
  
}

void ResetVars() {
    number_of_digits = 0;
    max_digits = 3;
    reading_state = 0;
    my_input = "";
}

void CheckButtons() {
  Button* pButton = NULL;
  const uint16_t numButtons = matrix.getNumButtons();

  if (matrix.update())
  {  
      // There was a change in any of the buttons  

      // Scan all buttons, except the last one
      for (uint16_t idx = 0; idx < numButtons; idx++)
      {        
          pButton = matrix.getButton(idx);    
          if (pButton->fell())      
          {
              // Button has rose. The fell() method automatically resets the event so you cannot call it twice for the same occurrence of the event!                 
             Serial.print("Button fell ");Serial.println(pButton->getNumber());
             if(pButton->getNumber() < 10) {   //This delegates the buttons dealing with the numbers. Button 11 and 12 could be mode buttons. Use another If statement
               timeout = millis() + my_timeout;
               Mode = 1;  //Switches to button mode in the main loop
               ButtonPressed(pButton->getNumber());  //Plays DTMF Tone and Flashes LED Red
               int mybtn = pButton->getNumber();
               number_of_digits++;
               switch (mybtn) {
                 case 0:  //1 was pressed
                   my_input = my_input + "1";
                   break;
                 case 1:  //2 was pressed
                   my_input = my_input + "2";
                   break;
                 case 2:  //3 was pressed
                   my_input = my_input + "3";
                   break;
                 case 3:  //4 was pressed
                   my_input = my_input + "4";
                   break;
                 case 4:  //5 was pressed
                   my_input = my_input + "5";
                   break;
                 case 5:  //6 was pressed
                   my_input = my_input + "6";
                   break;
                 case 6:  //7 was pressed
                   my_input = my_input + "7";
                   break;
                 case 7:  //8 was pressed
                   my_input = my_input + "8";
                   break;
                 case 8:  //9 was pressed
                   my_input = my_input + "9";
                   break;
                 case 9:  //0 was pressed
                   my_input = my_input + "0";
                   break;
               }
               if(number_of_digits > max_digits - 1) { CheckValid(); }   //Check if the current string length meets the right length, if so, check against correct values.
             }
             if(pButton->getNumber() == 11) {
               if(millis() < 2000) {
                 POST(100);
               } else {
                 ModeButton = 1;
               }
             }
           if(pButton->getNumber() == 10) {
              PressStar();
               }           
          }
          else if (pButton->rose())
          {
              // Button has rose. The rose() method automatically resets the event so you cannot call it twice for the same occurrence of the event! 
              Serial.print("Button rose ");Serial.println(pButton->getNumber());
          }               
      }
  }  
}

void CheckValid() {

  if(my_input == "911" || my_input == "867" || my_input == "777" || my_input == "8675309" || my_input == "77733222") {
   if(my_input == "911") {
     ResetVars();
     Mode = 0;
     Animation = 3;     
   }
   if(my_input == "867") {
     max_digits = 7;
   }
   if(my_input == "777") {
     max_digits = 8;
   }
   if(my_input == "8675309") {
     ResetVars();
     Mode = 0;
     Animation = 4;      
   }
   if(my_input == "77733222") {
     ResetVars();
     Mode = 0;
     Animation = 5;      
   }    
  } else {
     ResetVars();
     Mode = 1;
  }
   
}

void white_chase(int post_delay){
    int post_wait = post_delay;
    
    for(int i = 0; i < LED_Count; i++) {   
        ModeButton = 0; 
        CheckButtons();
        if(ModeButton == 1) { break; }
        digitalWrite(Anodes[i], HIGH);
        analogWrite(Blue, 0);
        analogWrite(Green, 128);
        analogWrite(Red, 200);
        delay(post_wait);
        analogWrite(Blue, 255);
        analogWrite(Green, 255);
        analogWrite(Red, 255);
        digitalWrite(Anodes[i], LOW);        
    }   
}

void reset(){
  digitalWrite(Red, HIGH);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue, HIGH);

  for(int i = 0; i < LED_Count; i++) {
    //pinMode(Anodes[i], OUTPUT);
    digitalWrite(Anodes[i], LOW);
  }
}

void rainbow() {

 int FADESPEED = 4;
 for(int i = 0; i < LED_Count; i++) {
  digitalWrite(Anodes[i], HIGH);
 }

  switch (Steps) {
  //switch(random(6)){
    //r = 255, g = 255, b = 0
    //r ++ = 3
    //g ++ = 5
    //b ++ = 1
    //r -- = 0
    //g -- = 2
    //b -- = 4


    
    case 3:
      // fade from blue to violet
      for (r = 0; r < 255; r++) {
        ModeButton = 0; CheckButtons(); if(ModeButton == 1) { break; }
        analogWrite(Red, r);
        delay(FADESPEED); 
      } 
      break;
    case 4:
      for (b = 255; b > 0; b--) { 
        ModeButton = 0; CheckButtons(); if(ModeButton == 1) { break; }
        analogWrite(Blue, b);
        delay(FADESPEED-3);
      } 
      break;
    case 5:
      for (g = 0; g < 225; g++) { 
        ModeButton = 0; CheckButtons(); if(ModeButton == 1) { break; }
        analogWrite(Green, g);
        delay(FADESPEED-3);
      } 
      break; 
    case 0:
      for (r = 255; r > 0; r--) { 
        ModeButton = 0; CheckButtons(); if(ModeButton == 1) { break; }
        analogWrite(Red, r);
        delay(FADESPEED);
      } 
      break;
    case 1:
      for (b = 0; b < 256; b++) {
        ModeButton = 0; CheckButtons(); if(ModeButton == 1) { break; }
        analogWrite(Blue, b);
        delay(FADESPEED+3);
      } 
      break;    
    case 2:
      for (g = 225; g > 0; g--) { 
        ModeButton = 0; CheckButtons(); if(ModeButton == 1) { break; }
        analogWrite(Green, g);
        delay(FADESPEED-3);
      } 
      break;
  }
  
}
//Power On Self-Test
void POST(int post_delay){
    int post_wait = post_delay;

    for(int i = 0; i < LED_Count; i++) {   
        digitalWrite(Anodes[i], HIGH);
        digitalWrite(Red, LOW);
        delay(post_wait);
        digitalWrite(Red, HIGH);
        digitalWrite(Anodes[i], LOW);
        
    }   
    for(int i = 0; i < LED_Count; i++) {   
        digitalWrite(Anodes[i], HIGH);
        digitalWrite(Green, LOW);
        delay(post_wait);
        digitalWrite(Green, HIGH);
        digitalWrite(Anodes[i], LOW);
    }   
    for(int i = 0; i < LED_Count; i++) {   
        digitalWrite(Anodes[i], HIGH);
        digitalWrite(Blue, LOW);
        delay(post_wait);
        digitalWrite(Blue, HIGH);
        digitalWrite(Anodes[i], LOW);
    }   
}
   

//Resets all anodes low/off
void reset_leds(){
  for(int i = 0; i < LED_Count; i++) {
    digitalWrite(Anodes[i], LOW);
  }
  digitalWrite(Red,   HIGH);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue,  HIGH);
}

//************************************************************************
//************************************************************************
//************************************************************************

void ButtonPressed(int whichbtn) {
  //digitalWrite(Red, LOW);

  
  DTMF_OUT[0] = Bin[whichbtn][0];
  DTMF_OUT[1] = Bin[whichbtn][1];
  DTMF_OUT[2] = Bin[whichbtn][2];
  DTMF_OUT[3] = Bin[whichbtn][3];
    

  mcp.digitalWrite(DTMF_Data_0, DTMF_OUT[0]);
  mcp.digitalWrite(DTMF_Data_1, DTMF_OUT[1]);
  mcp.digitalWrite(DTMF_Data_2, DTMF_OUT[2]);
  mcp.digitalWrite(DTMF_Data_3, DTMF_OUT[3]);
  

  digitalWrite(HT9200_CE, LOW);
  digitalWrite(PAM_CE, HIGH);
  digitalWrite(Anodes[whichbtn],HIGH);
  digitalWrite(Red, LOW);   delay(250); digitalWrite(Red, HIGH);
  //digitalWrite(Green, LOW); delay(200); digitalWrite(Green, HIGH);
  //digitalWrite(Blue, LOW);  delay(200); digitalWrite(Blue, HIGH);

  //delay(500);

  digitalWrite(HT9200_CE, HIGH);
  digitalWrite(PAM_CE, LOW);
  digitalWrite(Anodes[whichbtn],LOW);
  delay(50);
  reset();
}





void TestDTMF(){
  
  for (int i = 0; i < 10; i++){
    DTMF_OUT[0] = Bin[i][0];
    DTMF_OUT[1] = Bin[i][1];
    DTMF_OUT[2] = Bin[i][2];
    DTMF_OUT[3] = Bin[i][3];
    

    mcp.digitalWrite(DTMF_Data_0, DTMF_OUT[0]);
    mcp.digitalWrite(DTMF_Data_1, DTMF_OUT[1]);
    mcp.digitalWrite(DTMF_Data_2, DTMF_OUT[2]);
    mcp.digitalWrite(DTMF_Data_3, DTMF_OUT[3]);

/*
    mcp.digitalWrite(DTMF_Data_0, 1);
    mcp.digitalWrite(DTMF_Data_1, 1);
    mcp.digitalWrite(DTMF_Data_2, 0);
    mcp.digitalWrite(DTMF_Data_3, 0);
*/


    digitalWrite(HT9200_CE, LOW);
    //digitalWrite(PAM_CE, HIGH);

    digitalWrite(Anodes[i],HIGH);
    digitalWrite(Red, LOW);   delay(200); digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW); delay(200); digitalWrite(Green, HIGH);
    digitalWrite(Blue, LOW);  delay(200); digitalWrite(Blue, HIGH);
    //delay(1000);

    digitalWrite(HT9200_CE, HIGH);
    //digitalWrite(PAM_CE, LOW);

    digitalWrite(Anodes[i],LOW);
    delay(250);
  } 
}

void MakeTone(int n, int t){
    DTMF_OUT[0] = Bin[n][0];
    DTMF_OUT[1] = Bin[n][1];
    DTMF_OUT[2] = Bin[n][2];
    DTMF_OUT[3] = Bin[n][3];
    
    mcp.digitalWrite(DTMF_Data_0, DTMF_OUT[0]);
    mcp.digitalWrite(DTMF_Data_1, DTMF_OUT[1]);
    mcp.digitalWrite(DTMF_Data_2, DTMF_OUT[2]);
    mcp.digitalWrite(DTMF_Data_3, DTMF_OUT[3]);

    digitalWrite(HT9200_CE, LOW);
    digitalWrite(PAM_CE, HIGH);

    if (n == 0){
     n = 10;
    }
    else if(n == 11){
      n = random(9);
    }
    
    n=n-1;
    digitalWrite(Anodes[n],HIGH);
    digitalWrite(Red, LOW);   delay(t/3); digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW); delay(t/3); digitalWrite(Green, HIGH);
    digitalWrite(Blue, LOW);  delay(t/3); digitalWrite(Blue, HIGH);
    //delay(1000);

    digitalWrite(HT9200_CE, HIGH);
    digitalWrite(PAM_CE, LOW);

    digitalWrite(Anodes[n],LOW);
    delay(50);
  } 




void DoLouie(){
    MakeTone(1,250);
    MakeTone(1,250);
    MakeTone(1,250);
    delay(400);
    MakeTone(6,250);
    MakeTone(6,250);
    delay(250);
    MakeTone(9,250);
    MakeTone(9,250);
    MakeTone(9,250);
    delay(400);
    MakeTone(6,250);
    MakeTone(6,250);
    delay(250);
    CheckButtons();

}

void partyMode(){
  for(int i = 0; i < LED_Count; i++) {
    digitalWrite(Anodes[i], HIGH);
    //delay(3);
    //digitalWrite(Anodes[i], LOW);
  }
  
  int c = random(noColors);
  r = color[c][0];
  g = color[c][1];
  b = color[c][2];
  analogWrite(Red, r); analogWrite(Green, g); analogWrite(Blue, b);
  delay(250);
  
  CheckButtons();  
}


//************************************************************************
//************************************************************************
//************************************************************************

void PressStar(){
  MakeTone(10,250);
  
}
