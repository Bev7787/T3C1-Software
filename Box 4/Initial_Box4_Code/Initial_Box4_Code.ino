#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

unsigned long startLeftMillis;
unsigned long startRightMillis;

int redColor = 100;
int greenColor = 100;
int blueColor = 100;

//--------------------------------------------\\

int pathSwitchLeftIndicatorLightPin = 3;
int pathSwitchRightIndicatorLightPin = 2;

Servo topServo;
int topServoPin = 8;
int topServoPos = 1;

int topSensorState = 0;
int topSensorLastState = HIGH;
int topSensorPin  = 12;

int spiralStripPin = 5;
int spiralNumStripPixels = 32;
int spiralLightSequence = 0;

bool changeStripColour = false;
bool rotateTopServo = false;
bool changeSpiralColour = false;

//--------------------------------------------\\

Servo leftRetraceServo;
int leftRetraceServoPin = 10;
int leftRetraceServoPos = 0;

int leftSensorState = 1;
int leftSensorLastState = 0;
int leftSensorPin = 11;

int leftRetraceStripPin = 9;
int leftRetraceNumStripPixels = 10;

bool leftBallStationary = false;
bool runLeftRetrace = false;
bool rotateLeftServo = false;

//--------------------------------------------\\

Servo rightRetraceServo;
int rightRetraceServoPin = 7;
int rightRetraceServoPos = 0;

int rightSensorState = 1;
int rightSensorLastState = 0;
int rightSensorPin = 13;

int rightRetraceStripPin = 6;
int rightRetraceNumStripPixels = 10;

bool rightBallStationary = false;
bool runRightRetrace = false;
bool rotateRightServo = false;

//--------------------------------------------\\

Adafruit_NeoPixel spiralStrip(spiralNumStripPixels , spiralStripPin , NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftRetraceStrip(leftRetraceNumStripPixels, leftRetraceStripPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRetraceStrip(rightRetraceNumStripPixels, rightRetraceStripPin, NEO_GRB + NEO_KHZ800);

//--------------------------------------------------------------------------------------------------------------\\

void setup(){
  Serial.begin(9600);
  unsigned long currentTime = millis();
  
  digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
  digitalWrite(pathSwitchRightIndicatorLightPin, LOW);
  
  spiralStrip.begin();
  changeTopStripCol();

  topServo.attach(topServoPin);
  topServo.write(0);
  
  leftRetraceServo.attach (leftRetraceServoPin);
  leftRetraceServo.write(0);

  leftRetraceStrip.begin();
  leftRetraceRedColour();

  rightRetraceServo.attach (rightRetraceServoPin);
  rightRetraceServo.write(0);

  rightRetraceStrip.begin();
  rightRetraceRedColour();

  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  pinMode(topSensorPin, INPUT);

  PCICR |= B00000001;
  PCMSK0 |= B00111000;
}

void loop(){
  if(changeStripColour == true){
    setColor();
    changeTopStripCol();
    changeStripColour = false;
  }
  
  if (runLeftRetrace == true){
    if(leftBallStationary == true){
      leftStationaryBall();
      leftBallStationary = false;
    }

    if(millis() - startLeftMillis >= 1000 && rotateLeftServo == true){
      leftReleaseBall();
      rotateLeftServo = false;
    }

    if(millis() - startLeftMillis >= 3000){ //bufefr
      leftRetraceRedColour();
      runLeftRetrace = false;
    }
  }

  if (runRightRetrace == true){
    if(rightBallStationary == true){
      rightStationaryBall();
      rightBallStationary = false;
    }

    if(millis() - startRightMillis >= 1000 && rotateRightServo == true){
      rightReleaseBall();
      rotateRightServo = false;
    }

    if(millis() - startRightMillis >= 3000){ //bufefr
      rightRetraceRedColour();
      runRightRetrace = false;
    }
  }
}

ISR(PCINT0_vect){
  if (digitalRead(topSensorPin) == HIGH){
    changeStripColour = true;
    rotateTopServo = true;
    changeSpiralColour = true;
  }
  
  if (digitalRead(leftSensorPin) == HIGH){
    startLeftMillis = millis();
    leftBallStationary = true;
    rotateLeftServo = true;
    runLeftRetrace = true;
  }

  if (digitalRead(rightSensorPin) == HIGH){
    startRightMillis = millis();
    rightBallStationary = true;
    rotateRightServo = true;
    runRightRetrace = true;
  }
}

//--------------------------------------------------------------------------------------------------------------\\

void setColor(){
  redColor = random(0, 255);
  greenColor = random(0,255);
  blueColor = random(0, 255);
}

void changeTopServo(int topServoPos){
  if(topServoPos % 2 == 0){ //for every second ball, the servo will rotate 90 degrees 
      topServo.write(90);
    digitalWrite(pathSwitchRightIndicatorLightPin, LOW);  
    digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
    }
  if(topServoPos % 2 != 0){
      topServo.write(0);
      digitalWrite(pathSwitchLeftIndicatorLightPin, LOW);
  	  digitalWrite(pathSwitchRightIndicatorLightPin, HIGH);
    }
}

void changeTopStripCol() {
  setColor();
  if (spiralLightSequence == 0) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(redColor, greenColor, blueColor));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }
  
  else if (spiralLightSequence == 1) {
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

//--------------------------------------------------------------------------------------------------------------\\

void rightStationaryBall(){
  for (int i = 0; i < rightRetraceNumStripPixels; i++){
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 0, 255));
    rightRetraceStrip.show();
  }
}

void rightReleaseBall(){
  for (int i = 0; i < rightRetraceNumStripPixels; i++){
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 255, 0));
    rightRetraceStrip.show();
  }
  rightRetraceServo.write(180);
}

void rightRetraceRedColour() {
  for (int i = 0; i < rightRetraceNumStripPixels; i++){
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(255, 0, 0));
    rightRetraceStrip.show();
  }
  rightRetraceServo.write(0);
}

//--------------------------------------------------------------------------------------------------------------\\

void leftStationaryBall(){
  //Blue LED strip when ball is stationary
  for (int i = 0; i < leftRetraceNumStripPixels; i++){
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 0, 255));
    leftRetraceStrip.show();
  }
}

void leftReleaseBall(){
  for (int i = 0; i < leftRetraceNumStripPixels; i++){
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 255, 0));
    leftRetraceStrip.show();
  }
  leftRetraceServo.write(180);
}

void leftRetraceRedColour() {
  for (int i = 0; i < leftRetraceNumStripPixels; i++){
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(255, 0, 0));
    leftRetraceStrip.show();
  }
  leftRetraceServo.write(0);
}
