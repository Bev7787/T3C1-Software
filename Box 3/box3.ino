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
bool runmotor1 = false;
bool runmotor2 = false;

bool retraceLightLeft = false;
bool retraceLightRight = false;
// bool spiralLed = false;

// ball sensor pins placeholder
int ballPin1 = A0;
int ballPin2 = A1;

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
  pinMode(motorSen1, INPUT_PULLUP);
  pinMode(motorSen2, INPUT_PULLUP);
  pinMode(ballPin1, INPUT);
  pinMode(ballPin2, INPUT);

  // Sensor modes set
  digitalWrite(ballPin1, LOW);
  digitalWrite(ballPin2, LOW);

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

  // Motor off by default
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  // Setup marble sensor interrupts.
  PCICR |= B00000010;
  PCMSK1 |= B00000011;

  // Attach reed switch interrupts
  attachInterrupt(digitalPinToInterrupt(motorSen1), leftRetracePath, RISING);
  attachInterrupt(digitalPinToInterrupt(motorSen2), rightRetracePath, RISING);
}

ISR(PCINT1_vect)
{
  if (digitalRead(ballPin1) == HIGH)
  {
    // set boolean
    balltimestamp1 = millis();
    runmotor1 = true;
  }
  if (digitalRead(ballPin2) == HIGH)
  {
    // set boolean
    balltimestamp2 = millis();
    runmotor2 = true;
  }
}

void leftRetracePath()
{
  runmotor1 = false;
  retraceLightLeft = true;
  Ball bl = {millis()};
  //ballsInSystemQueueL.add(bl);
}

void rightRetracePath()
{
  runmotor2 = false;
  retraceLightRight = true;
  Ball br = {millis()};
  //ballsInSystemQueueR.add(br);
}

void runMotor(int in1, int in2, bool runmotor)
{
  if (runmotor == true)
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

void loop()
{

  if ((millis() - balltimestamp1) >= 1000)
    runMotor(in1, in2, runmotor1);
  if ((millis() - balltimestamp2) >= 1000)
    runMotor(in3, in4, runmotor2);

  if (retraceLightLeft)
  {
    // get the ball from head of the queue
    Ball bl = ballsInSystemQueueL.get(0);
    if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME)
    {
      // TODO: LED left func
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
      // TODO: LED right func
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
      // TODO: LED spiral func
      ballsInSpiral.remove(0);
    }
  }
}

