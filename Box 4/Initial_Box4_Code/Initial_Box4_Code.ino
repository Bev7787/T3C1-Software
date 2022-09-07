#include <Adafruit_NeoPixel.h>
#include <Servo.h> 


Servo myServo;

int sensorState = 0;
int lastState = HIGH;
int sensorPin = 1;

int servoPin = 2;
int servoPos = 0;

int stripPin = 3;
int numStripPixels = 10;    // change this value to however many pixels we have in the strip
int lightSequence = 0;


Adafruit_NeoPixel pixels(numStripPixels, stripPin, NEO_GRB + NEO_KHZ800);




void setup() {
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  myServo.attach(servoPin);
  myServo.write(seroPos);

  pixels.begin();

  

  Serial.being(9600);

}

void loop() {
  sensorState = digialRead(sensorPin);

  if (sensorState == LOW && lastState == HIGH) {
    topServoFunc();
    changeTopStripCol();
  }


  lastState = sensorState;


}


void topServoFunc() {
  if (servoPos == 0) {
    myservo.write(90);
    servoPos = 90;
  }

  if (servoPos == 90) {
    myservo.write(0);
    seroPos = 0;
  }
}


void changeTopStripCol() {
  if (lightSequence == 0) {
    for (int i = 0; i < numStripPixels; i++) {
      pixels.setPixelColor(255, 0, 0);
      pixels.show();
    }
    lightSequence ++;
  }
  
  else if (lightSequence == 1) {
    for (int i = 0; i < numStripPixels; i++) {
      pixels.setPixelColor(0, 255, 0);
      pixels.show();
    }
    lightSequence ++;
  }
  
  else {
    for (int i = 0; i < numStripPixels; i++) {
      pixels.setPixelColor(0, 0, 255);
      pixels.show();
    }
    lightSequence ++;
  }
}
