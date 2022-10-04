#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

int topSensorState = 0;
int topSensorLastState = HIGH;
int topSensorPin  = 12;

Servo topServo;
int topServoPin = 13;
int topServoPos = 1; //keeps incrementing everytime the button is pressed and the lights change

int spiralStripPin = 5;
int spiralNumStripPixels = 32; // change this value to however many pixels we have in the strip
int spiralLightSequence = 0;

Adafruit_NeoPixel spiralStrip(spiralNumStripPixels , spiralStripPin , NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(topSensorPin, INPUT);
  digitalWrite(topSensorPin, HIGH);
  spiralStrip.begin();
//  topServo.write(0);
  topServo.attach(topServoPin);
  
  Serial.begin(9600);
}

void loop() {
  topSensorState = digitalRead(topSensorPin);

  if (topSensorState == LOW && topSensorLastState == HIGH) {
    Serial.println("Ball Passed Through Sensor");
    changeTopStripCol();
  }

  topSensorLastState = topSensorState;
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
  if (spiralLightSequence  == 0) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(255, 0, 0));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }
  
  else if (spiralLightSequence  == 1) {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
      spiralStrip.setPixelColor(i, spiralStrip.Color(0, 255, 0));
      spiralStrip.show();
    }
    spiralLightSequence++;
    topServoPos++;
  }
  
  else {
    changeTopServo(topServoPos);
    for (int i = 0; i < spiralNumStripPixels ; i++) {
     spiralStrip.setPixelColor(i, spiralStrip.Color(0, 0, 255));
     spiralStrip.show();
    }
    spiralLightSequence = 0;
    topServoPos++;
  }
}