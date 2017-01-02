#include <delay.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// External methods 
void gameInit();
void gameSetup();
void moveBlockDown(int time);

// Initializers for push buttons
const int btn1B = PUSH1; // Button 1 on Tiva board 
const int btn2B = PUSH2; // Button 2 on Tiva board

// Arrays for position of cursor on pages
const int cursorMain[] = {16,26}; // Y-coordinate of cursor on main menu
const int cursorDiff[] = {20,42,64,86,108,90}; // X-coordinate of cursor on difficulty page

// Global variables
int cursor = 0; // determines position of cursor to see which option was selected 
 
// Enum for all game pages. Sets the current page to "main"
static enum Pages{
  Main = 0,
  Difficulty = 1,
  Game = 2
} currPage = Main;

// Methods for drawing pages 
void drawMain(){
  OrbitOledClear();
  currPage = Main; 
  OrbitOledMoveTo(13,cursorMain[0]);
  OrbitOledDrawRect(15,cursorMain[0] + 2);

  OrbitOledMoveTo(25,2);
   OrbitOledDrawString("**TETRIS**");
   
  OrbitOledMoveTo(20,14);
  OrbitOledDrawString("Start!");

  OrbitOledMoveTo(20,24);
  OrbitOledDrawString("Difficulty");
 
  OrbitOledUpdate();
}

void drawDifficulty(){
   currPage = Difficulty; 
   OrbitOledMoveTo(cursorDiff[0] - 5, 9);
   OrbitOledDrawRect(cursorDiff[0] - 3, 11);
  
   // Draws the different level values
   OrbitOledMoveTo(cursorDiff[0],7);
   OrbitOledDrawString("1"); 

   OrbitOledMoveTo(cursorDiff[1],7);
   OrbitOledDrawString("2"); 

   OrbitOledMoveTo(cursorDiff[2],7);
   OrbitOledDrawString("3"); 

   OrbitOledMoveTo(cursorDiff[3],7);
   OrbitOledDrawString("4"); 

   OrbitOledMoveTo(cursorDiff[4],7);
   OrbitOledDrawString("5"); 

   OrbitOledMoveTo(cursorDiff[5],18);
   OrbitOledDrawString("Back"); 
}


void newCursorPos(){ // Changes position of cursor, so that if the cursor exceeds the last available option on page, will go back to the first option
  size_t arraySize; 
  if (currPage == Main) arraySize = sizeof(cursorMain); 
  else  arraySize = sizeof(cursorDiff); 
      
  arraySize = arraySize/sizeof(int) - 1;

  if (cursor < arraySize){
    cursor++;
  } else {
    cursor = 0;
  }
}

void moveMainCursor(){ // Moves cursor on "main" page 
  OrbitOledMoveTo(13,cursorMain[cursor]);
  OrbitOledFillRect(15, cursorMain[cursor] + 2);
 
  newCursorPos(); 
  
  OrbitOledMoveTo(13,cursorMain[cursor]);
  OrbitOledDrawRect(15,cursorMain[cursor] + 2);
  
  OrbitOledUpdate();
}

void moveDiffCursor(){ // Moves cursor on "difficulty" page 
   if (cursor < 5){
     OrbitOledMoveTo(cursorDiff[cursor] - 5, 9);
     OrbitOledFillRect(cursorDiff[cursor] - 3 , 11);
   } else {
     OrbitOledMoveTo(cursorDiff[cursor] - 5, 21);
     OrbitOledFillRect(cursorDiff[cursor] - 3 , 23);
   }
  
   newCursorPos();

   if (cursor < 5) {
    OrbitOledMoveTo(cursorDiff[cursor] - 5, 9);
    OrbitOledDrawRect(cursorDiff[cursor] - 3 , 11);
   } else {
    OrbitOledMoveTo(cursorDiff[cursor] - 5, 21);
    OrbitOledDrawRect(cursorDiff[cursor] - 3 , 23);
   }
   
   OrbitOledUpdate();
}

void selectDiff(){ // Keeps track of what difficulty was chosen
  if (cursor != 5){
    // LED flashes when difficulty is selected 
    pinMode(LED2,OUTPUT);
    pinMode(LED1,OUTPUT);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    delay(100);
  }
  switch (cursor){
    case 0:
      timeinterval = 500;
      break;
    case 1:
      timeinterval = 400;
      break;
    case 2:
      timeinterval = 300;
      break;
    case 3:
      timeinterval = 200;
      break;
    case 4:
      timeinterval = 50;
      break;
    case 5:
      drawMain();
      cursor = 0;
      break;
  }
}

void changeScreen(){ 
  OrbitOledClear();
  switch(cursor){
    case 0:
      currPage = Game; 
      gameSetup(); // sets up pins and draws background 
      break;
    case 1:
      drawDifficulty();
      break;
  }
  cursor = 0;
  OrbitOledUpdate();
}

void pagesSetup(){
  // Set up for the buttons on the Tiva board
  pinMode(btn2B, INPUT_PULLUP);//2B
  pinMode(btn1B, INPUT_PULLUP);//1B
  
  // Draw the main menu
  drawMain(); 
  
  OrbitOledUpdate();
}

void buttonSwitch() { //Changes what is done when buttons are pressed on different pages 
  switch (currPage){
    case Main:
      attachInterrupt(PUSH1, changeScreen, FALLING);
      attachInterrupt(PUSH2, moveMainCursor, FALLING);
      break;
    case Difficulty:
      attachInterrupt(PUSH1, selectDiff, FALLING);
      attachInterrupt(PUSH2, moveDiffCursor, FALLING);
      break; 
    case Game:
    // Detach previous interrupts associated with push buttons 
     detachInterrupt(PUSH1);
     detachInterrupt(PUSH2);

     // Interrupts for gameplay
     attachInterrupt(PD_2,moveBlockLeft, RISING);
     attachInterrupt(PE_0, moveBlockRight, RISING);
     attachInterrupt(PUSH1, rotate, FALLING);
     
     buttonState4 = digitalRead(btn4); // for soft drops
     if(buttonState4==LOW) moveBlockDown(0); 
     
     gameInit(); // actual gameplay
     break; 
    }
} 

