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

// LED definitions
#define LED1 GPIO_PIN_1 
#define LED RED_LED
#define LED2 BLUE_LED

// Push button variables
const uint32_t btn1=PD_2, btn2=PE_0;
const int btn3 = PUSH1,btn4=PUSH2;
int buttonState1 = 0,buttonState2 = 0,buttonState3= 0,buttonState4= 0;
const int numCol = 6, numRow = 28;

// Global variables
int timeinterval = 500; // used to increase/decrease speed blocks fall at (default is 500ms)
int point = 0; // keeps track of points
int a = 0, b = 24; //cursor position starts around left corner 
int randNum; // used to generate random numbers to randomize blocks
int (*currBlock)[4][4][4] = NULL; // pointer to indicate the current block on screen
int rotateNum = 0; // keeps track of which rotation position the current block is at 

// Arrays representing each block's shape, including their rotations
int Ishape[4][4][4]={
  {{1,1,1,1},  {0,0,0,0},  {0,0,0,0},  {0,0,0,0}},
  {{1,0,0,0},  {1,0,0,0},  {1,0,0,0},  {1,0,0,0}},
  {{1,1,1,1},  {0,0,0,0},  {0,0,0,0},  {0,0,0,0}},
  {{1,0,0,0},  {1,0,0,0},  {1,0,0,0},  {1,0,0,0}}};  

int Jshape[4][4][4]={
  {{1,1,1,1},
  {1,0,0,0}},
  {{0,1,1,0},
  {0,0,1,0},
  {0,0,1,0},
  {0,0,1,0}},
  {{0,0,0,1},
   {1,1,1,1}},
  {{0,1,0,0},
  {0,1,0,0},
  {0,1,0,0},
  {0,1,1,0}}  
  };

int Lshape[4][4][4]={{
  {1,0,0,0},
  {1,1,1,1}},
  {
  {0,1,1,0},
  {0,1,0,0},
  {0,1,0,0},
  {0,1,0,0}},
  {
  {1,1,1,1},
  {0,0,0,1}},
  {
  {0,0,1,0},
  {0,0,1,0},
  {0,0,1,0},
  {0,1,1,0}}
  };

int Oshape[4][4][4]={{
  {1,1},
  {1,1}},
  {
  {1,1},
  {1,1}},
  {
  {1,1},
  {1,1}},
  {
  {1,1},
  {1,1}}};

int Zshape[4][4][4]={{
  {1,1,0,0},
  {0,1,1,0}},
  {
  {0,0,1,0},
  {0,1,1,0},
  {0,1,0,0}},
  {
  {1,1,0,0},
  {0,1,1,0}},
  {
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0}}};

int Tshape[4][4][4]={{
  {0,1,0},
  {1,1,1}},
  {
  {0,1,0},
  {0,1,1},
  {0,1,0}},
  {
  {1,1,1},
  {0,1,0}},
  {
  {0,1,0},
  {1,1,0},
  {0,1,0}}};
 
int Sshape[4][4][4]={{
  {0,1,1},
  {1,1,0}},
  {
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0}},
  {
  {0,1,1},
  {1,1,0}},
  {
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0}}};

// Array to represent background 
int background[numRow][numCol]={
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0},
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0},
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0},
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}, 
{0,0,0,0,0,0}};

void gameSetup() { // method for setting up pins and general gameplay (draws background and initial random block)
  randomSeed(analogRead(0)); // used to generate a completely random number by using unused pin
  
  // Pin setup 
  //pinMode(LED1,OUTPUT);
  pinMode(btn1, INPUT); //move left
  pinMode(btn2, INPUT); //move right

  // LED pinmodes
  pinMode(LED1, OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(LED2, OUTPUT);

  drawBackground();
  randomBlock();
  OrbitOledUpdate();
}

void gameInit() { // General game logic (reading value of buttons, moving block down and executing different functions depending on which button is pressed)
  digitalWrite(LED1, LOW); 
  digitalWrite(LED,LOW);
  digitalWrite(LED2, LOW);

  moveBlockDown(timeinterval); // function to move block down (blocks will move down at different speeds, depending on timeInterval's value 
}

void randomBlock(){
 randNum = (int) random(7); // gets random value between 0 and 6
 rotateNum = 0; // reset block to default orientation
 OrbitOledMoveTo(a,b); 

 switch(randNum){
  case 0:
    currBlock = &Ishape; 
    break; 
  case 1:
    currBlock = &Jshape; 
    break; 
  case 2:
    currBlock = &Lshape; 
    break; 
  case 3:
    currBlock = &Oshape; 
    break; 
  case 4:
    currBlock = &Zshape; 
    break; 
  case 5:
    currBlock = &Tshape; 
    break; 
  case 6:
    currBlock = &Sshape; 
    break;
 }
  drawShape(currBlock,rotateNum);
  OrbitOledUpdate();
}

void rotate(){ // changes orientation of current block
  if(rotateNum<3){
      rotateNum++;
    }else{
      rotateNum = 0;
    }
  }

void moveBlockDown(int intervaltime){ 
 if(lose() == 1){ // player loses
  OrbitOledClear();
  OrbitOledMoveTo(10,10); 
  OrbitOledDrawString("**Game Over**");
  printPoint();
  OrbitOledUpdate();
  } else {
    if(checkCollision(currBlock,rotateNum) == 0 && checkSideCollision(currBlock, rotateNum) == 0){ // Can move block down 
      OrbitOledClear();
      drawBackground();
      drawShape(currBlock, rotateNum); 
      a+=4;
      OrbitOledUpdate();
      delay(intervaltime);
    } else { // block has landed
      a-= 4;
      addLanded(currBlock,rotateNum);
      if (clearLines() > 0){ // at least 1 line was cleared
        OrbitOledClear();
        drawBackground();
        OrbitOledUpdate();
      }
      a = 0;
      b = 24;
      randomBlock(); //Block landed --> generate new one  
    }
  }
}

void printPoint(){
  int newPoint = point;
  int x = 115; 
  OrbitOledMoveTo(x,22);
 
  do{ // takes digit of point variable and outputs onto screen
     OrbitOledDrawChar('0'+ (newPoint%10)); 
     newPoint /= 10;
     x-=8;
     OrbitOledMoveTo(x,22);
  } while (newPoint > 0);
}

int lose(){
  for(int i = 0; i < numCol; i++){ // if have a block on topmost row, then game over 
    if(background[0][i]==1) return 1;
  }
  return 0;
}

void moveBlockLeft(){
  b+=4;  
  if(checkSideCollision(currBlock,rotateNum) == 0){ // if can move block left 
    OrbitOledClear();
    drawBackground();  
    drawShape(currBlock,rotateNum); //block->random generated
    OrbitOledUpdate();
  } else {
      b-=4;  
  }
}

void moveBlockRight(){ // if can move block right 
  b-=4;
  if(checkSideCollision(currBlock,rotateNum) == 0){
    OrbitOledClear();
    drawShape(currBlock,rotateNum); //block->random generated
    drawBackground();    
  
    OrbitOledUpdate();
    } else {
       b+=4;
    }
}

void addLanded(int(*shape)[4][4][4],int rotateNum){ // adds landed block to background array
 int x = a/4, y = (24-b)/4; // position of cursor on Oled relative to background array
  for (int r = 0; r < 4; r++){
    for (int c = 0; c < 4; c++){
      if ((*shape)[rotateNum][r][c] == 1) background[x][y] = 1; //changes value on background array to show that there is a block there now
      y++; // moves to next column in background array
    } 
    y = (24-b)/4; // returns to original position of index for drawing shape
    x++; // moves down a row 
  }
  OrbitOledClear();
  drawBackground();
  OrbitOledUpdate();
}

int checkCollision(int (*shape)[4][4][4], int rotateNum){ // checks collisions underneath falling block
  int x=a/4,y=(24-b)/4; // position of cursor on Oled relative to background array

  for (int r = 0; r < 4 && x < numRow; r++){
    for (int c = 0; c < 4 && y < numCol; c++){
      if ((*shape)[rotateNum][r][c] == 1 && background[x][y] == 1) return -1; // if have a block at an element of background array which equals to 1, then have a collision
      y++; // increments the column val being checked for background array
    }
    y = (24-b)/4; // initial location of cursor when drawing block 
    x++;  // increases the row being compared in background array
  }
  return 0;
}

int checkSideCollision(int (*shape)[4][4][4], int rotateNum){ // Ensures block does not move out of bounds of screen (left, right and bottom row)
  int y = (24-b)/4, x = a/4; //relates current cursor position of block to background array indices 
  for (int r = 0; r < 4; r++){
    for (int c = 0; c < 4; c++){
      if ((*shape)[rotateNum][r][c] == 1 && (y > 5 || y < 0 || x > 27)) return -1; // if collides with another block or exceeds boundaries of screen
      y++;
    }
    y = (24-b)/4;
    x++;
  }
  return 0;
}

void drawShape(int (*shape)[4][4][4], int rotateNum){
  int x=a, y=b;
  for (int r = 0; r < 4; r++){
    for (int c = 0; c < 4; c++){
      if ((*shape)[rotateNum][r][c] == 1){ // if value of array is 1, draw a rectangle at given position
        OrbitOledMoveTo(x,y);        
        OrbitOledDrawRect(x+4, y-4);              
      }
      y-=4;  
    }
    y = b;
    x += 4; 
  }
}

void drawBackground(){
  int x,y; 
  for(int r = 0; r < numRow; r++){
    for(int c = 0; c < numCol; c++){
      if(background[r][c] == 1){ // if have 1, draw a rectangle at given position
        x = r*4;
        y = 24-(c*4);
        OrbitOledMoveTo(x, y);
        OrbitOledDrawRect(x+4, y-4);
      }
    }    
  }
}

int clearLines(){ 
  int canClear = 0; // if can clear a line
  int cleared = 0; // if did clear a line

  for (int r = 0; r < numRow; r++){
    for (int c = 0; c < numCol; c++){
      if (background[r][c] == 1){ // if have a 1, then have the potential of clearing a line 
        canClear = 1;
      } else {
        canClear = 0; // if do not have 1, then know that line cannot be cleared 
        break;
      }
    }
    if (canClear == 1){ // if a line can be cleared 
      for (int i = 0 ;i < 3;i++){ // blink LED 3 times to notify user 
        digitalWrite(LED1, HIGH);
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED1, LOW);
        digitalWrite(LED, LOW);
        delay(100);
       } 
       point+=10; // increment number of points 
       
       for (int c = 0; c < numCol; c++){ // set background row to 0 to indicate row is now gone 
        background[r][c] = 0;
       }
       
      for (int x = r - 1; x > -1; x--){ //moves all blocks above deleted row downwards 
        for (int y = 0; y < numCol; y++){
          if (background[x][y] == 1){
            background[x][y] = 0;
            background[x+1][y] = 1;
          }
        }
      }
    }
  }
  return cleared; // tells moveDownMethod that a row has been cleared, and so background should be updated 
}
