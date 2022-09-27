#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

int leftSensorState = 1;
int leftSensorLastState = 0;
int leftSensorPin = 11;

Servo leftRetraceServo;
int leftRetraceServoPin = 10;
int leftServoFlag = 0;

int LeftRetraceStripPin = 9;
int LeftRetraceNumStripPixels = 10;    // change this value to however many pixels we have in the strip

Adafruit_NeoPixel leftRetraceStrip(LeftRetraceNumStripPixels, LeftRetraceStripPin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(leftSensorPin, INPUT);
  //digitalWrite(leftSensorPin, LOW);
  
  leftRetraceStrip.begin();
  
  leftRetraceServo.attach(leftRetraceServoPin);
  leftRetraceServo.write(0);
  
  Serial.begin(9600);
}

void loop() {
  leftSensorState = digitalRead(leftSensorPin);

  if (leftSensorState == HIGH) {//if sensor is activated, turn green
    releaseBall();
  }
  else{
    leftRetraceRedColour();//else retrace stip remain red
  }
}

void releaseBall(){
  rotateRetraceServo();
  leftRetraceGreenColour();
}

void rotateRetraceServo(){
  if (leftSensorState == HIGH)
  {
    leftServoFlag++;
  }
  int x = 0;
  if(leftServoFlag == 1)
    x = 0;
  else if(leftServoFlag == 2)
    x = 180;
    else {
    x = 0;
  	leftServoFlag = 0;
  }

  leftRetraceServo.write(x);
  delay(1000);
}

void leftRetraceGreenColour() {
  for (int i = 0; i < LeftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 255, 0));
    leftRetraceStrip.show();
  }
}

void leftRetraceRedColour() {
  for (int i = 0; i < LeftRetraceNumStripPixels; i++) {
    leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(255, 0, 0));
    leftRetraceStrip.show();
  }
}
