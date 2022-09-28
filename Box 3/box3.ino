// C++ code
// Library used: https://www.arduino.cc/reference/en/libraries/adafruit-neopixel/
#include <Adafruit_NeoPixel.h>

unsigned long motortimestamp1 = 0;
int ballSensor1 = 0;
unsigned long motortimestamp2 = 0;
int ballSensor2 = 0;
int ballSen1State = 0;
int ballSen2State = 0;
int lastState1 = HIGH;
int lastState2 = HIGH;

// ball sensor pins placeholder
int ballPin1 = 8;
int ballPin2 = 9;

// Motor A connections
int enA = 1;
int in1 = 4;
int in2 = 7;
int motorSen1 = 2;
// Motor B connections
int enB = 9;
int in3 = 10;
int in4 = 15;
int motorSen2 = 3;

void setup()
{
  // Set sensors
  pinMode(motorSen1, INPUT);
  pinMode(motorSen2, INPUT);
  pinMode(ballPin1, INPUT);
  pinMode(ballPin2, INPUT);
  
  // Sensor modes set
  digitalWrite(motorSen1, HIGH);
  digitalWrite(motorSen2, HIGH);
  digitalWrite(ballPin1, HIGH);
  digitalWrite(ballPin2, HIGH);
  
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Set speed
  analogWrite(enA, 255);
  analogWrite(enB, 255);
	
  // Move motors to starting position. Can be removed
  if (digitalRead(motorSen1) == LOW)
  	runMotor(in1, in2, motorSen1);
  if (digitalRead(motorSen2) == LOW)
  	runMotor(in3, in4, motorSen2);
}

void loop()
{
  int ballSen1State = digitalRead(ballPin1);
  int ballSen2State = digitalRead(ballPin2);
  
  if (ballSensor1 == LOW && lastState1 == HIGH) 
   motortimestamp1 = millis(); 
  else if (ballSensor2 == LOW && lastState2 == HIGH)
   motortimestamp2 = millis() + 1000; 
  lastState1 = ballSen1State;
  lastState2 = ballSen2State;
  
  if ((unsigned char)(millis() - motortimestamp) >= 1000)
    runMotor(in1, in2, motorSen1);
  //else if (millis() >= motortimestamp2)
  //  runMotor(in3, in4, motorSen2);
}

void runMotor(int in1, int in2, int pin) {
 	  // Run once to move away from sensor.
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    // Run until sensor detected.
  	int motorSenState = digitalRead(pin);
  	while (motorSenState == LOW) {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      motorSenState = digitalRead(pin)
    } 
    // Turn off motors.
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}