#include <Adafruit_NeoPixel.h>
#include <Servo.h> 


Servo myServo;

int sensorState = 0;
int lastState = HIGH;
int sensorPin = 7;

Servo servo_10;
int topServoPin = 2;
int topServoPos = 1; //keeps incrementing everytime the button is pressed and the lights change

int stripPin = 4;
int numStripPixels = 32;    // change this value to however many pixels we have in the strip
int lightSequence = 0;


Adafruit_NeoPixel pixels(numStripPixels, stripPin, NEO_GRB + NEO_KHZ800);


void setup() {
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pixels.begin();
  servo_10.attach(10, 500, 2500);
  
  Serial.begin(9600);
}

void loop() {
  sensorState = digitalRead(sensorPin);

  if (sensorState == LOW && lastState == HIGH) {
    Serial.println("Ball Passed Through Sensor");
    changeTopStripCol();
  }

  lastState = sensorState;
}

void changeTopServo(int topServoPos){
  if(topServoPos % 2 == 0){ //for every second ball, the servo will rotate 90 degrees 
      servo_10.write(90);
    }
  if(topServoPos % 2 != 0){
      servo_10.write(0);
    }

}

void changeTopStripCol() {
  if (lightSequence == 0) {
    changeTopServo(topServoPos);
    for (int i = 0; i < numStripPixels; i++) {
      pixels.setPixelColor(i, pixels.Color(random(1, 254), 0, 0));
      pixels.show();
    }
    lightSequence ++;
    topServoPos++;
  }
  
  else if (lightSequence == 1) {
    changeTopServo(topServoPos);
    for (int i = 0; i < numStripPixels; i++) {
      pixels.setPixelColor(i, pixels.Color(0, random(1, 254), 0));
      pixels.show();
    }
    lightSequence ++;
    topServoPos++;
  }
  
  else {
    changeTopServo(topServoPos);
    for (int i = 0; i < numStripPixels; i++) {
     pixels.setPixelColor(i, pixels.Color(0, 0, random(1, 254)));
      pixels.show();
    }
    lightSequence =0;
    topServoPos++;
  }
}