// C++ code
// Library used: https://www.arduino.cc/reference/en/libraries/adafruit-neopixel/
#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>

#define RETRACE_TIME = 1000; // Placeholder value
#define SPIRAL_TIME = 2000;  // Placeholder value

volatile unsigned long balltimestamp1 = 0;
volatile unsigned long balltimestamp2 = 0;
volatile bool runmotor1 = false;
volatile bool runmotor2 = false;

volatile bool retraceLightLeft = false;
volatile bool retraceLightRight = false;

// LED configuration
int stripPin = 12;
int leftRetraceNumStripPixels = 5; // Placeholder numbers until actual number of LEDs are confirmed.
int rightRetraceNumStripPixels = 5;
int spiralStripPixels = 10;
Adafruit_NeoPixel ledStrips(leftRetraceNumStripPixels + rightRetraceNumStripPixels + spiralStripPixels, 
                            leftRetraceStripPin, NEO_GRB + NEO_KHZ800);

// Placeholder indicator LED pins.
int leftIndicator = A2;
int rightIndicator = A3;

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

volatile LinkedList<Ball>
    ballsInSystemQueueL = LinkedList<Ball>(); // left path queue
volatile LinkedList<Ball>
    ballsInSystemQueueR = LinkedList<Ball>(); // right path queue

LinkedList<Ball> ballsInSpiral = LinkedList<Ball>(); // spiral path queue


void setup()
{
  // Set sensors
  pinMode(motorSen1, INPUT_PULLUP);
  pinMode(motorSen2, INPUT_PULLUP);
  pinMode(ballPin1, INPUT);
  pinMode(ballPin2, INPUT);

  // Set LEDs
  pinMode(stripPin, OUTPUT);
  pinMode(leftIndicator, OUTPUT);
  pinMode(rightIndicator, OUTPUT);

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

void loop()
{

  // Run motors after waiting 1 second.
  if ((millis() - balltimestamp1) >= 1000)
    runMotor(in1, in2, runmotor1);
  if ((millis() - balltimestamp2) >= 1000)
    runMotor(in3, in4, runmotor2);

  // Determining whether to run LEDs based on flags.
  if (retraceLightLeft)
  {
    // get the ball from head of the queue
    Ball bl = ballsInSystemQueueL.get(0);
    if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME)
    {
      leftPathLED();
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
      rightPathLED();
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
      spiralLED();
      ballsInSpiral.remove(0);
    }
  }
}

ISR(PCINT1_vect)
{
  if (digitalRead(ballPin1) == HIGH)
  {
    // set LEDs for left path
    digitalWrite(leftIndicator, HIGH);
    digitalWrite(rightIndicator, LOW);
    // set boolean
    balltimestamp1 = millis();
    runmotor1 = true;
  }
  if (digitalRead(ballPin2) == HIGH)
  {
    // set LEDs for right path
    digitalWrite(leftIndicator, LOW);
    digitalWrite(rightIndicator, HIGH);
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
  ballsInSystemQueueL.add(bl);
}

void rightRetracePath()
{
  runmotor2 = false;
  retraceLightRight = true;
  Ball br = {millis()};
  ballsInSystemQueueR.add(br);
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

// LED functions. The order is currently assumed to be spiral -> left -> right.
// TODO: Add colours
// TODO: Turn off left path, right path LEDs.
void spiralLED()
{
  for (int i = 0; i < spiralStripPixels; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
}

void leftPathLED()
{
  for (int i = spiralStripPixels; i < leftRetraceNumStripPixels + spiralStripPixels; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
}

void rightPathLED()
{
  for (int i = leftRetraceNumStripPixels + spiralStripPixels; 
           i < rightRetraceNumStripPixels + leftRetraceNumStripPixels + spiralStripPixels; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
  }
}

