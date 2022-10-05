// C++ code
// Library used: https://www.arduino.cc/reference/en/libraries/adafruit-neopixel/
#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>

#define RETRACE_TIME = 1000; // TO DO: get the exact value
#define SPIRAL_TIME = 2000;  // TO DO: get the exact value

unsigned long motortimestamp1 = 0;
unsigned long motortimestamp2 = 0;
unsigned long balltimestamp1 = 0;
unsigned long balltimestamp2 = 0;
bool motorsenstate1 = false;
bool motorsenstate2 = false;

bool retraceLightLeft = false;
bool retraceLightRight = false;
// bool spiralLed = false;

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

struct Ball
{
  unsigned long reedSwitchTimestamp;
};

LinkedList<Ball>
    ballsInSystemQueueL = LinkedList<Ball>(); // left path queue
LinkedList<Ball>
    ballsInSystemQueueR = LinkedList<Ball>(); // right path queue

LinkedList<Ball> ballsInSpiral = LinkedList<Ball>(); // spiral path queue

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
  attachInterrupt(digitalPinToInterrupt(motorSen1), leftRetracePath, RISING);
  attachInterrupt(digitalPinToInterrupt(motorSen2), rightRetracePath, RISING);
}

ISR(PCINT1_vect)
{
  if (digitalRead(ballPin1) == LOW)
  {
    // set boolean
    balltimestamp1 = millis();
    motorsenstate1 = true;
  }
  if (digitalRead(ballPin2) == LOW)
  {
    // set boolean
    balltimestamp2 = millis();
    motorsenstate2 = true;
  }
}

void leftRetracePath()
{
  motorsenstate1 = false;
  retraceLightLeft = true;
  Ball bl = {millis()};
  ballsInSystemQueueL.add(bl);
}

void rightRetracePath()
{
  motorsenstate2 = false;
  retraceLightRight = true;
  Ball br = {millis()};
  ballsInSystemQueueR.add(br);
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

  if (retraceLightLeft)
  {
    // get the ball from head of the queue
    Ball bl = ballsInSystemQueueL.get(0);
    if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME)
    {
      retraceLightLeft = false;
      Ball removedBall = ballsInSystemQueueL.remove(0);
      ballsInSpiral.add(removedBall);
    }
  }

  if (retraceLightRight)
  {
    // get the ball from head of the queue
    Ball br = ballsInSystemQueueR.get(0);
    if (millis() - br.reedSwitchTimestamp >= RETRACE_TIME)
    {
      retraceLightRight = false;
      Ball removedBall = ballsInSystemQueueR.remove(0);
      ballsInSpiral.add(removedBall);
    }
  }

  if (ballsInSpiral.size() > 0) // there is something in spiral
  {
    Ball bl = ballsInSpiral.get(0);
    if (millis() - bl.reedSwitchTimestamp >= SPIRAL_TIME)
    {
      ballsInSpiral.remove(0);
    }
  }
}

void runMotor(int in1, int in2, bool motorsen)
{
  // TODO: Replace motorsensor and ballsensor with interrupts.
  if (motorsen == true)
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else
  {
    // Turn off motors.
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}