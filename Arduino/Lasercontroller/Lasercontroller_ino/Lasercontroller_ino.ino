/*
Arduino as a Controller for a Lasercutter made out of 2 Scanner

 Works together with another Programm called Lasersnap
 

 by Florian Albrecht
 
 */

#include <Stepper.h>

#define Laserpin 3 //The Pin on which the Relaise for the Laser is attached


#define motorSteps 200     // change this depending on the number of steps
#define motorStepsz 200
// per revolution of your motor
#define motorPin1 12
#define motorPin2 11
#define motorPin3 10
#define motorPin4 9
#define ledPin 13
#define motorPin1z 7
#define motorPin2z 6
#define motorPin3z 5
#define motorPin4z 4
// initialize of the Stepper library:
Stepper myStepperx(200, 12,11,10,9); 
Stepper mySteppery(200, 4,5,6,7);
void setup() {
  // set the motor speed at 60 RPMS:
  myStepperx.setSpeed(80);
  mySteppery.setSpeed(80);
  // Initialize the Serial port:
  Serial.begin(9600);

  // set up the LED pin:
  pinMode(ledPin, OUTPUT);
  // blink the LED:
  pinMode(Laserpin, OUTPUT);
  blink(3);
}

int nowx = 0, nowy = 0; //position of Laser


float xyratio = 1.4; //ratio of gears of y and x
int t = 1;
double get = 0;


void loop() {

  if(Serial.available()>0){
    int coordx, coordy; //receiving coordinates
    boolean statuss;
    int stat = Serial.read();
    /*if(stat == 79){ //if first character ist O Laser will not be on
      statuss = false;
      Serial.print("on");
    }*/
    if(stat == 49){ //if first character ist P Laser will be turned on
      statuss = true;
    }
      else{
        statuss = false;
    }
    Serial.read();
    delay(40);
    Serial.print("Status:");
    Serial.print(statuss);
    coordx = getint();
    Serial.print("  x:");
    Serial.print(coordx);
    delay(40);
    coordy = getint();
    Serial.print("  y:");
    Serial.println(coordy);
    movelaser(coordx, coordy, statuss);
  }
}

void movelaser(int x, int y, boolean ison){
  int movex, movey;
  movex = x - nowx;
  movey = y - nowy;
  int directionx, directiony;
  if(ison){
    digitalWrite(Laserpin, HIGH);
  }
  if(movex > 0){
    directionx = 1;
   }
   else {
    directionx = -1;
    movex *= -1;
  }
  if(movey > 0){
    directiony = 1;
    }
    else{
      directiony = -1;
      movey *= -1;
    }
  int steppedx = 0;
  int steppedy = 0;
  if(movex > movey){
    myStepperx.step(directionx);
    steppedx++;
    nowx += directionx;
    }
    else{
      mySteppery.step(directiony);
      steppedy++;
      nowy += directiony;
  }
  while(steppedx != movex && steppedy != movey){
    if(steppedx/movex > steppedy){
      myStepperx.step(directionx);
      steppedx++;
      nowx += directionx;
      }
      else{
        mySteppery.step(directiony);
        steppedy++;
        nowy += directiony;
     }
  }
  if(ison){
    digitalWrite(Laserpin, LOW);
  }
}

// Blink the reset LED:
void blink(int howManyTimes) {
  int i;
  for (i=0; i< howManyTimes; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}


int getint(){
  boolean minus = false;
  double l = 1;
  int number = 0;
  int get[20];
  int i = 0;
  for(i = 0; i < 3; i){
      get[i] = Serial.read() - 48;
      if(get[i] < 11){
      if(get[i] == -3){
        minus = true;
      }
      else{
      i++;
      }
      delay(30);
      }
    }
    i--;
    if(i > 0){
      int a = i;
      while(a > 0){
        l *= 10;
        a--;
      }
    }      
    double c = 1;
    while(i>=0){
      number = number + get[i]*c;
      c *= 10;
      i--;
    }
  if(minus){
    number *= -1;
  }
  return number;
}
