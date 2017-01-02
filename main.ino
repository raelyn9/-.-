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

void pagesSetup();
void buttonSwitch(); // changes  what buttons do based on the page 
void gameSetup();
void gameInit();

void setup() {
  //Generic setup
  OrbitOledInit();
  OrbitOledClear();
  OrbitOledClearBuffer();
  
  pinMode(LED1,OUTPUT);
  pagesSetup();  // draws the pages  
}

void loop(){
  buttonSwitch(); // changes what the buttons do depending on what page player is on (includes the actual gameplay)
} 
