#include <GyverOLED.h>
#include "GyverEncoder.h"

#define encoder0Pin5V 7
#define encoder0PinGND 8

#define encoder0PinA 2 
#define encoder0PinB 3 
#define encoder0Button 4 

#define OLED_SDA 5
#define OLED_SCL 6

#define led_pin 12      

#define step_freq 1
#define step_rough 10

volatile int time_light = 2;
volatile int freq = 25; 

uint8_t state = 0;

Encoder enc1(encoder0PinA, encoder0PinB, encoder0Button, TYPE2);
GyverOLED oled(0x3C);


unsigned long timer;
bool StrobeState;

void setup() 
{   
  pinMode(led_pin, OUTPUT);
  pinMode(encoder0Pin5V, OUTPUT);
  pinMode(encoder0PinGND, OUTPUT);
  pinMode(OLED_SDA, OUTPUT);
  pinMode(OLED_SCL, OUTPUT);
  digitalWrite(encoder0Pin5V, 1);
  digitalWrite(encoder0PinGND, 0);
  digitalWrite(OLED_SDA, 1);
  digitalWrite(OLED_SCL, 1);
  delay(500);
  oledInit();
  Serial.begin(9600);
  updateScreen();
  timer = millis();
  StrobeState = false;
} 

void loop()
{ 
  enc1.tick();
  processEnc();
  checkVar();
  blink();
}





void processEnc()
{
  if(enc1.isTurn())
  {
    processRotate();
  }
  else if(enc1.isClick())
  {
    processPress();
  }
}
void processPress()
{
  if(state==1)
  {
    state=0;
  }
  else if(state==0)
  {
    state=1;
  }
  updateScreen();
}
void processRotate()
{
  if(enc1.isRightH())
  {
    changeVar(step_freq);
  }
  else if(enc1.isRight())
  {
    changeVar(step_freq*step_rough);
  }
  else if(enc1.isLeftH())
  {
    changeVar(-(step_freq));
  }
  else if(enc1.isLeft())
  {
    changeVar(-step_freq*step_rough);
  }
}
void changeVar(int mills)
{
  if(state == 0)
  {
    freq += mills;
    Serial.println(freq);
  }
  else if(state == 1)
  {
    time_light += mills;
    Serial.println(time_light);
  }
  
  updateScreen();
}
void blink()
{
  if(!StrobeState)
  {  
    if(millis() - timer >= 1000/freq-time_light)
    {
      timer = millis();
      StrobeState = !StrobeState;
      digitalWrite(led_pin, StrobeState);
    }
  }
  else if(StrobeState)
  {
    if(millis()-timer>= time_light)
    {
      timer = millis();
      StrobeState = !StrobeState;
      digitalWrite(led_pin, StrobeState);
    }
  }
}
void checkVar()
{
  if(freq<0)
  {
    freq=0;
    updateScreen();
  }
  if(time_light<0)
  {
    time_light = 0;
    updateScreen();
  }
}
void oledInit() 
{
  Wire.begin();
  Wire.setClock(400000L);
  oled.init(OLED128x64);
  oled.setContrast(3);
  oled.clear();
  oled.home();
  oled.scale2X();
  oled.println("Strobe");
  oled.scale1X();
  oled.println("v0.2by Les Torgovets");
  oled.println();
  delay(1000);
}

void updateScreen()
{
  if(state == 0)
  {
    printFreq();
  }
  else if(state == 1)
  {
    printTime();
  }
}

void printFreq()
{
  oled.clear();
  oled.home();
  oled.scale2X();
  oled.println("Frequency");
  oled.scale1X();
  oled.println(freq);
}

void printTime()
{
  oled.clear();
  oled.home();
  oled.scale2X();
  oled.println("Light time");
  oled.scale1X();
  oled.println(time_light);
}