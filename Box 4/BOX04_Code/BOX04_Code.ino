#include <Adafruit_NeoPixel.h>
#include <Servo.h>

/*
  To run the code using Buttons:
    - Comment out lines 98, 109, 120
    - Change values from LOW to HIGH on lines 178, 182, 190

  When runnin the code with IR Break Beam Sensors, you must ensure that:
    - Lines 98, 109, 120 are NOT commented out
    - Values on lines 178, 182, 190 are checking for LOW not HIGH (i.e == LOW)
*/

int redColour = 0;
int greenColour = 0;
int blueColour = 0;

int retraceAngle01 = 10;
int retraceAngle02 = 70;

int RightRetraceAngle01 = 210;
int RightRetraceAngle02 = 120;

//----------------------------\\

Servo topServo;
int topServoPin = 8;
int topServoPos = 0;

int spiralAngle1 = 122;
int spiralAngle2 = 72;

int topSensorPin = 12;

int spiralStripPin = 5;
int spiralNumStripPixels = 15;
int spiralLightSequence = 0;

volatile bool changeStripColour = false;
volatile unsigned long startTopMillis;

int pathSwitchLeftIndicatorLightPin = 3;
int pathSwitchRightIndicatorLightPin = 2;

//----------------------------\\

Servo leftRetraceServo;
int leftRetraceServoPin = 7;
int leftRetraceServoPos = 0;

int leftSensorPin = 11;

int leftRetraceStripPin = 9;
int leftRetraceNumStripPixels = 4;

volatile bool leftBallStationary = false;
volatile bool runLeftRetrace = false;
volatile bool rotateLeftServo = false;
volatile bool leftReleaseFlag = false;

volatile unsigned long startLeftMillis;

//----------------------------\\

Servo rightRetraceServo;
int rightRetraceServoPin = 6;
int rightRetraceServoPos = 0;

int rightSensorPin = 13;

int rightRetraceStripPin = 4;
int rightRetraceNumStripPixels = 4;

volatile bool rightBallStationary = false;
volatile bool runRightRetrace = false;
volatile bool rotateRightServo = false;
volatile bool rightReleaseFlag = false;

volatile unsigned long startRightMillis;

//----------------------------\\

Adafruit_NeoPixel spiralStrip(spiralNumStripPixels, spiralStripPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftRetraceStrip(leftRetraceNumStripPixels, leftRetraceStripPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRetraceStrip(rightRetraceNumStripPixels, rightRetraceStripPin, NEO_GRB + NEO_KHZ800);

//----------------------------------------------------------------------------------------------------------------------------------------\\

void setup() {
  unsigned long currentTime = millis();
pinMode(pathSwitchLeftIndicatorLightPin, OUTPUT);
pinMode(pathSwitchRightIndicatorLightPin, OUTPUT);

  digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
  digitalWrite(pathSwitchRightIndicatorLightPin, LOW);

  spiralStrip.begin();
  changeTopStripColour();

  topServo.attach(topServoPin);
  topServo.write(spiralAngle1);

  pinMode(topSensorPin, INPUT);
  digitalWrite(topSensorPin, HIGH);

  //----------------------------\\

  leftRetraceServo.attach(leftRetraceServoPin);
  leftRetraceServo.write(retraceAngle01);

  leftRetraceStrip.begin();
  leftRetraceRedColour();

  pinMode(leftSensorPin, INPUT);
  digitalWrite(leftSensorPin, HIGH);

  //----------------------------\\

  rightRetraceServo.attach(rightRetraceServoPin);
  rightRetraceServo.write(RightRetraceAngle01);

  rightRetraceStrip.begin();
  rightRetraceRedColour();

  pinMode(rightSensorPin, INPUT);
  digitalWrite(rightSensorPin, HIGH);

  //----------------------------\\

  PCICR |= B00000001;
  PCMSK0 |= B00111000;
}

void loop() {
  if (changeStripColour == true) {
    setColour();
    if(millis() - startTopMillis >= 250){
      changeTopStripColour();
      startTopMillis =  millis();
      changeStripColour = false;
    }
    
  }

  if (runLeftRetrace == true) {
    if (leftBallStationary == true) {
      leftStationaryBall();
      leftBallStationary = false;
    }

    if (millis() - startLeftMillis >= 1000 && rotateLeftServo == true) {
      leftReleaseBall();
      rotateLeftServo = false;
    }

    if (millis() - startLeftMillis >= 1500 && leftReleaseFlag == true) {
      leftRetraceRedColour();
      runLeftRetrace = false;
      leftReleaseFlag = false;
    }
  }

  if (runRightRetrace == true) {
    if (rightBallStationary == true) {
      rightStationaryBall();
      rightBallStationary = false;
    }

    if (millis() - startRightMillis >= 1000 && rotateRightServo == true) {
      rightReleaseBall();
      rotateRightServo = false;
    }

    if (millis() - startRightMillis >= 1500 && rightReleaseFlag == true) {
      rightRetraceRedColour();
      runRightRetrace = false;
      rightReleaseFlag = false;
    }
  }
}

/*
  The purpose of this function is to detect any changes in state
  of the input pins (sensor pins) and set the corresponding bool
  variables to true and record current time (where applicable)
 */
ISR(PCINT0_vect) {
  if (digitalRead(topSensorPin) == LOW) {
    startTopMillis = millis();
    changeStripColour = true;
  }

  if (digitalRead(leftSensorPin) == LOW && rotateLeftServo == false) {
    startLeftMillis = millis();
    leftBallStationary = true;
    rotateLeftServo = true;
    runLeftRetrace = true;
    leftReleaseFlag = true;
  }

  if (digitalRead(rightSensorPin) == LOW && rotateRightServo == false) {
    startRightMillis = millis();
    rightBallStationary = true;
    rotateRightServo = true;
    runRightRetrace = true;
    rightReleaseFlag = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------\\

/*
  The purpose of this function is to randomly generate a new colour for
  the strip lights
 */
void setColour() {
  redColour = random(0, 255);
  greenColour = random(0, 255);
  blueColour = random(0, 255);
}

/*
  The purpose of this function is to rotate the top servo to switch the 
  path the ball takes between left path and right path and turn on the
  corresponding led indicator light bulb
 */
void changeTopServo(int topServoPos) {
  if (topServoPos % 2 == 0) {
    topServo.write(spiralAngle1);
    digitalWrite(pathSwitchRightIndicatorLightPin, LOW);
    digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
  }
  else {
    topServo.write(spiralAngle2);
    digitalWrite(pathSwitchRightIndicatorLightPin, HIGH);
    digitalWrite(pathSwitchLeftIndicatorLightPin, LOW);
  }
}

/*
  The purpose of this function is to change the colour of the top spiral light
  and call the function to rotate the top servo
 */
void changeTopStripColour() {
  setColour();
  if (spiralLightSequence == 0) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(255, 0, 0));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }

  else if (spiralLightSequence == 1) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(0, 255, 0));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }

  else {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(0, 0, 255));
      spiralStrip.show();
    }
    spiralLightSequence = 0;
    topServoPos++;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------\\

/*
  The purpose of this function is to show the marble is stationary by turning the
  retrace light strip to blue
 */
void leftStationaryBall() {
  for (int i = 0; i < leftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 0, 255));
    leftRetraceStrip.show();
  }
}

/*
  The purpose of this function is to show the marble is moving again by turning
  the light strip to green and rotating the servo
 */
void leftReleaseBall() {
  for (int i = 0; i < leftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 255, 0));
    leftRetraceStrip.show();
  }
  leftRetraceServo.write(retraceAngle01);
}

/*
  The purpose of this function is to show that the ball cannot pass through by turning
  the light strip to red and rotating the servo up
 */
void leftRetraceRedColour() {
  for (int i = 0; i < leftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(255, 0, 0));
    leftRetraceStrip.show();
  }
  leftRetraceServo.write(retraceAngle02);
}

//----------------------------------------------------------------------------------------------------------------------------------------\\

/*
  The purpose of this function is to show the marble is stationary by turning the
  retrace light strip to blue
 */
void rightStationaryBall() {
  for (int i = 0; i < rightRetraceNumStripPixels; i++) {
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 0, 255));
    rightRetraceStrip.show();
  }
}

/*
  The purpose of this function is to show the marble is moving again by turning
  the light strip to green and rotating the servo
 */
void rightReleaseBall() {
  for (int i = 0; i < rightRetraceNumStripPixels; i++) {
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 255, 0));
    rightRetraceStrip.show();
  }
  rightRetraceServo.write(RightRetraceAngle01);
}

/*
  The purpose of this function is to show that the ball cannot pass through by turning
  the light strip to red and rotating the servo up
 */
void rightRetraceRedColour() {
  for (int i = 0; i < rightRetraceNumStripPixels; i++) {
    rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(255, 0, 0));
    rightRetraceStrip.show();
  }
  rightRetraceServo.write(RightRetraceAngle02);
}