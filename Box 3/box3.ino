// C++ code
// Library used: https://www.arduino.cc/reference/en/libraries/adafruit-neopixel/
#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>

unsigned long motortimestamp1 = 0;
unsigned long motortimestamp2 = 0;
unsigned long balltimestamp1 = 0;
unsigned long balltimestamp2 = 0;
bool motorsenstate1 = false;
bool motorsenstate2 = false;
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

long retraceTime = 1000;  // constant set for now
long timeToSpiral = 2000; // constant set for now

struct Ball
{
  unsigned long enterTimeTop;
  unsigned long enterRetracePath;
}

LinkedList<Ball>
    ballsInSystemQueueL = LinkedList<Ball>(); // left path queue
LinkedList<Ball>
    ballsInSystemQueueR = LinkedList<Ball>(); // right path queue

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

  // Setup marble sensor interrupts.
  PCICR |= B00000010;
  PMSK1 |= B00000011;

  // Attach reed switch interrupts
  attachInterrupt(digitalPinToInterrupt(motorSen1), leftPath, RISING);
  attachInterrupt(digitalPinToInterrupt(motorSen2), rightPath, RISING);
}

ISR (PCINT1_vect) {
  if (digitalRead(ballPin1) == LOW) {
    // set boolean
    balltimestamp1 = millis();
    motorsenstate1 = true;
  }
  if (digitalRead(ballPin2) == LOW) {
    // set boolean
    balltimestamp2 = millis();
    motorsenstate2 = true;
  }
}

void leftPath()
{
  motorsenstate1 = false;
  // set the enterRetracePath time by removing the first one from the queue
  /*Ball removedBallL = ballsInSystemQueueL.remove(0);
  removedBallL.enterRetracePath = millis() - removedBallL.enterTimeTop;
  if (millis() - removedBallL.enterRetracePath >= retraceTime)
  {
    // trigger sprial LEDs
  }*/
}

void rightPath()
{
  motorsenstate2 = false;
  // set the enterRetracePath time by removing the first one from the queue
  /*Ball removedBallR = ballsInSystemQueueR.remove(0);
  removedBallR.enterRetracePath = millis() - removedBallR.enterTimeTop;
  if (millis() - removedBallR.enterRetracePath >= retraceTime)
  {
    // trigger sprial LEDs
  }*/
}

void loop()
{
  /*int ballSen1State = digitalRead(ballPin1);
  int ballSen2State = digitalRead(ballPin2);

  if (ballSensor1 == LOW && lastState1 == HIGH)
    motortimestamp1 = millis() + 1000;

  else if (ballSensor2 == LOW && lastState2 == HIGH)
    motortimestamp2 = millis() + 1000;
  lastState1 = ballSen1State;
  lastState2 = ballSen2State;
  */
 // TODO: Implement interrupts
  if ((unsigned long)(millis() - balltimestamp1) >= 1000)
    runMotor(in1, in2, motorsenstate1);
  if ((unsigned long)(millis() - balltimestamp2) >= 1000)
    runMotor(in3, in4, motorsenstate2);
}

void runMotor(int in1, int in2, bool motorsen) {
  // TODO: Replace motorsensor and ballsensor with interrupts.
 	if (motorsen == true) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else {
    // Turn off motors.
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}