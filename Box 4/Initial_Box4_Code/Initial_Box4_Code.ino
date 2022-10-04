#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

const unsigned long ballStationary = 1000;
const unsigned long ballStationaryBuffer = 500;

unsigned long startMillis;
unsigned long currentMillis;
unsigned long previousTime = 0;

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

Servo leftRetraceServo;
int leftRetraceServoPin = 10;
int leftServoFlag = 1;
int leftRetraceServoPos = 0;

int leftSensorState = 1;
int leftSensorLastState = 0;
int leftSensorPin = 11;

int LeftRetraceStripPin = 9;
int LeftRetraceNumStripPixels = 10;    // change this value to however many pixels we have in the strip

//-----------------

Servo rightRetraceServo;
int rightRetraceServoPin = 7;
int rightServoFlag = 1;
int rightRetraceServoPos = 0;

int rightSensorState = 1;
int rightSensorLastState = 0;
int rightSensorPin = 8;

int rightRetraceStripPin = 6;
int rightRetraceNumStripPixels = 10;

//-----------------

Servo topServo;
int topServoPin = 13;
int topServoPos = 1; //keeps incrementing everytime the button is pressed and the lights change

int topSensorState = 0;
int topSensorLastState = HIGH;
int topSensorPin  = 12;

int spiralStripPin = 5;
int spiralNumStripPixels = 32; // change this value to however many pixels we have in the strip
int spiralLightSequence = 0;

Adafruit_NeoPixel spiralStrip(spiralNumStripPixels , spiralStripPin , NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftRetraceStrip(LeftRetraceNumStripPixels, LeftRetraceStripPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRetraceStrip(rightRetraceNumStripPixels, rightRetraceStripPin, NEO_GRB + NEO_KHZ800);

void setup()
{
  pinMode(leftSensorPin, INPUT);
  pinMode(topSensorPin, INPUT);
  
  Serial.begin(9600);
  unsigned long currentTime = millis();
  
  topServo.attach(topServoPin);
  
  leftRetraceServo.attach (leftRetraceServoPin);
  leftRetraceServo.write(0);
  
  rightRetraceServo.attach (rightRetraceServoPin);
  rightRetraceServo.write(0);
  
  spiralStrip.begin();
  
  leftRetraceStrip.begin();
  leftRetraceRedColour();
  
  rightRetraceStrip.begin();
  rightRetraceRedColour();
}

void loop()
{
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);

  if(digitalRead(leftSensorPin) == HIGH){
      rotateLeftRetraceServo();
  }

  if(digitalRead(rightSensorPin) == HIGH){
      rotateRightRetraceServo();
  }
  
  topSensorState = digitalRead(topSensorPin);

  if (topSensorState == LOW && topSensorLastState == HIGH) {
    Serial.println("Ball Passed Through Sensor");
    changeTopStripCol();
  }

  topSensorLastState = topSensorState;
}

//--------------------------------------------------------

void setColor(){
  redColor = random(0, 255);
  greenColor = random(0,255);
  blueColor = random(0, 255);
}

void changeTopServo(int topServoPos){
  if(topServoPos % 2 == 0){ //for every second ball, the servo will rotate 90 degrees 
      topServo.write(90);
    }
  if(topServoPos % 2 != 0){
      topServo.write(0);
    }
}

void changeTopStripCol() {
  setColor();
  if (spiralLightSequence  == 0) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(redColor, greenColor, blueColor));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }
  
  else if (spiralLightSequence  == 1) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(redColor, greenColor, blueColor));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }
  
  else {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
     spiralStrip.setPixelColor(i, spiralStrip.Color(redColor, greenColor, blueColor));
     spiralStrip.show();
    }
    spiralLightSequence = 0;
    topServoPos++;
  }
}

//--------------------------------------------------------

void rotateRightRetraceServo(){
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= ballStationary + ballStationaryBuffer) {
    if(rightServoFlag == 1){
      rightRetraceServo.write(0);
      rightServoFlag = 2;
      rightRetraceBlue2Green();
    }
    else if(rightServoFlag == 2){
      rightRetraceServo.write(180);
      rightServoFlag = 1;
      rightRetraceBlue2Green();
    }
    else {
      rightServoFlag = 0;
      Serial.println("Error! --> Function: rotateRightRetraceServo()");  
    }
    previousTime = currentTime;
  }
}

void rightRetraceBlue2Green() {
  //Blue when ball is stationary
  for (int i = 0; i < rightRetraceNumStripPixels; i++) {
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 0, 255));
    rightRetraceStrip.show();
  }
  
  delay(1000);
  
  //Green when ball begins rolling
  for (int i = 0; i < rightRetraceNumStripPixels; i++) {
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 255, 0));
    rightRetraceStrip.show();
  }
  
  delay(500);
  rightRetraceRedColour();
}

void rightRetraceRedColour() {
  for (int i = 0; i < rightRetraceNumStripPixels; i++) {
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(255, 0, 0));
    rightRetraceStrip.show();
}
}

//--------------------------------------------------------

void rotateLeftRetraceServo(){
  unsigned long currentTime = millis();
  if ( currentTime - previousTime >= ballStationary + ballStationaryBuffer) {
    if(leftServoFlag == 1){
      leftRetraceServo.write(0);
      leftServoFlag = 2;
      leftRetraceBlue2Green();
    }
    else if(leftServoFlag == 2){
      leftRetraceServo.write(180);
      leftServoFlag = 1;
      leftRetraceBlue2Green();
    }
    else {
      leftServoFlag = 0;
      Serial.println("Error! --> Function: rotateLeftRetraceServo()");  
    }
    previousTime = currentTime;
  }
}

void leftRetraceBlue2Green() {
  //Blue when ball is stationary
  for (int i = 0; i < LeftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 0, 255));
    leftRetraceStrip.show();
  }
  
  delay(1000);
  
  //Green when ball begins rolling
  for (int i = 0; i < LeftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 255, 0));
    leftRetraceStrip.show();
  }
  
  delay(500);
  leftRetraceRedColour();
}

void leftRetraceRedColour() {
  for (int i = 0; i < LeftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(255, 0, 0));
    leftRetraceStrip.show();
}
}

