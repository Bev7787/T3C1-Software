// C++ code
// Library used: https://www.arduino.cc/reference/en/libraries/adafruit-neopixel/
#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>

// Timers for LED operation.
// Defined from reed switch to start of component.
#define RETRACE_TIME = 1000; // Placeholder value
#define SPIRAL_TIME = 2000;  // Placeholder value

volatile unsigned long balltimestamp1 = 0;
volatile unsigned long balltimestamp2 = 0;
volatile bool runmotor1 = false;
volatile bool runmotor2 = false;

volatile bool retraceLightLeft = false;
volatile bool retraceLightRight = false;

// LED configuration
int stripPin = A2;
int leftRetraceNumStripPixels = 5; // Placeholder numbers until actual number of LEDs are confirmed.
int rightRetraceNumStripPixels = 5;
int spiralStripPixels = 10;
Adafruit_NeoPixel pixels(leftRetraceNumStripPixels + rightRetraceNumStripPixels + spiralStripPixels, 
                            stripPin, NEO_RGB + NEO_KHZ800);

// Spiral colours
int red = 0;
int green = 255;
int blue = 0;

// Indicator LED pins
int leftIndicator = A3;
int rightIndicator = A4;

// Marble sensor pins
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
  pinMode(leftIndicator, OUTPUT);
  pinMode(rightIndicator, OUTPUT);

  pixels.begin();
  spiralLED();
  leftPathLED(255, 0, 0);
  rightPathLED(255, 0, 0);

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

  // Run motors after allowing the marble to remain stationary for at least 1 second.
  if ((millis() - balltimestamp1) >= 1100)
    runMotor(in1, in2, runmotor1);
  if ((millis() - balltimestamp2) >= 1100)
    runMotor(in3, in4, runmotor2);

  /*  Determining whether to run LEDs based on flags.
      When a marble is about to move down the retrace,
      the LEDs will turn green for 200ms.
      Otherwise the LEDs are red.
  */
  if (retraceLightLeft)
  {
    // get the marble from head of the queue
    Ball bl = ballsInSystemQueueL.get(0);
    if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME)
    {
      // Check if LEDs have been on for at least 200ms. If so, turn to red.
      if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME + 200) 
      {
        ballsInSystemQueueL.shift();
        leftPathLED(255, 0, 0);
      }
      else 
      {
        leftPathLED(0, 255, 0);
        retraceLightLeft = false;
        ballsInSpiral.add(bl);
      }
    }
  }

  if (retraceLightRight)
  {
    Ball br = ballsInSystemQueueR.get(0);
    if (millis() - br.reedSwitchTimestamp >= RETRACE_TIME)
    {
      if (millis() - br.reedSwitchTimestamp >= RETRACE_TIME + 200) 
      {
        Ball removedBall = ballsInSystemQueueR.shift();
        rightPathLED(255, 0, 0);
      }
      else 
      {
        rightPathLED(0, 255, 0);
        retraceLightRight = false;
        ballsInSpiral.add(bl);
      }
    }
  }

  // Spiral LED. Runs if there are elements in spiral queue.
  if (ballsInSpiral.size() > 0)
  {
    Ball bl = ballsInSpiral.get(0);
    if (millis() - bl.reedSwitchTimestamp >= SPIRAL_TIME)
    {
      spiralLED();
      ballsInSpiral.shift();
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
    // set motors to run and current time.
    balltimestamp1 = millis();
    runmotor1 = true;
  }
  if (digitalRead(ballPin2) == HIGH)
  {
    // set LEDs for right path
    digitalWrite(leftIndicator, LOW);
    digitalWrite(rightIndicator, HIGH);
    // set motors to run and current time.
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

/*  Functions relating to the operation of LEDs.
    The order of LED strips is spiral -> left retrace -> right retrace.
*/
void spiralLED()
{
  randomColour();
  for (int i = 0; i < spiralStripPixels; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
    pixels.show();
  }
}

void leftPathLED(int r, int g, int b)
{
  for (int i = spiralStripPixels; i < leftRetraceNumStripPixels + spiralStripPixels; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
}

void rightPathLED(int r, int g, int b)
{
  for (int i = leftRetraceNumStripPixels + spiralStripPixels; 
           i < rightRetraceNumStripPixels + leftRetraceNumStripPixels + spiralStripPixels; i++) 
  {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
}

void randomColour(){
  red = random(0, 255);
  green = random(0,255);
  blue = random(0, 255);
}