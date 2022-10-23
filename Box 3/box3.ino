// C++ code for box 3.
#include <Adafruit_NeoPixel.h>
#include <LinkedList.h>

// Timers for LED operation.
// Defined from reed switch to start of component.
#define RETRACE_TIME = 1000; // Placeholder value
#define SPIRAL_TIME = 2000;  // Placeholder value
#define FIRST_HALF_SPIRAL = 1000;
#define HOLD_TIME = 1100;
#define MOTOR_RUN_TIME = 200;

// Timestamps utilised from 
volatile unsigned long ballTimeStamp1 = 0;
volatile unsigned long ballTimeStamp2 = 0;

volatile bool runMotor1 = false;
volatile bool runMotor2 = false;

volatile bool retraceLightLeft = false;
volatile bool retraceLightRight = false;

// Flags used to determine whether to execute certain operations in reed switch debouncing.
bool reset1 = false;
bool reset2 = false;

// LED strip configuration
int stripPin = A2;
int leftRetraceNumStripPixels = 3;
int rightRetraceNumStripPixels = 3;
int startSpiralStrip = 9;
int endSpiralStrip = 9;
Adafruit_NeoPixel pixels(leftRetraceNumStripPixels + rightRetraceNumStripPixels + startSpiralStrip + endSpiralStrip,
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
int enA = 10;
int in1 = 4;
int in2 = 5;
int motorSen1 = 2;
// Motor B connections
int enB = 11;
int in3 = 6;
int in4 = 7;
int motorSen2 = 3;

struct Ball
{
  unsigned long reedSwitchTimestamp;
};

LinkedList<Ball>
    ballsInSystemQueueL = LinkedList<Ball>(); // left path queue
LinkedList<Ball>
    ballsInSystemQueueR = LinkedList<Ball>(); // right path queue

LinkedList<Ball> ballsInSpiralFirstHalf = LinkedList<Ball>(); // spiral path queue first half

LinkedList<Ball> ballsInSpiralSecondHalf = LinkedList<Ball>(); // spiral path queue second half

void setup()
{
  // Set sensors
  pinMode(motorSen1, INPUT_PULLUP);
  pinMode(motorSen2, INPUT_PULLUP);
  pinMode(ballPin1, INPUT_PULLUP);
  pinMode(ballPin2, INPUT_PULLUP);

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

  // Motor off on initialisation.
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  // Setup marble sensor interrupts.
  PCICR |= B00000010;
  PCMSK1 |= B00000011;

  // Attach reed switch interrupts
  attachInterrupt(digitalPinToInterrupt(motorSen1), leftRetracePath, CHANGE);
  attachInterrupt(digitalPinToInterrupt(motorSen2), rightRetracePath, CHANGE);
}

void loop()
{
  // Debounce reed switch input.
  debounceReedInput(runMotor1, ballTimeStamp1, retraceLightLeft, ballsInSystemQueueL, reset1);
  debounceReedInput(runMotor2, ballTimeStamp2, retraceLightRight, ballsInSystemQueueR, reset2);

  // Run motors after allowing the marble to remain stationary for at least 1 second.
  if ((millis() - ballTimeStamp1) >= HOLD_TIME)
    motor(in1, in2, runMotor1);
  if ((millis() - ballTimeStamp2) >= HOLD_TIME)
    motor(in3, in4, runMotor2);

  // Run the LEDs on the retrace ramps.
  if (retraceLightLeft)
    runRetrace(retraceLightLeft, ballsInSystemQueueL, 0);
  if (retraceLightRight)
    runRetrace(retraceLightRight, ballsInSystemQueueR, 1);

  // Spiral LED. Runs if there are elements in spiral queue.
  if (ballsInSpiralFirstHalf.size() > 0)
  {
    Ball bl = ballsInSpiralFirstHalf.get(0);
    if (millis() - bl.reedSwitchTimestamp >= FIRST_HALF_SPIRAL)
    {
      spiralLED(0);
      ballsInSpiralFirstHalf.shift();
      ballsInSpiralSecondHalf.add(bl);
    }
  }

  if (ballsInSpiralSecondHalf.size() > 0)
  {
    Ball bl = ballsInSpiralSecondHalf.get(0);
    if (millis() - bl.reedSwitchTimestamp >= SPIRAL_TIME)
    {
      spiralLED(1);
      ballsInSpiralSecondHalf.shift();
    }
  }
}

// ISR that is triggered by a change in state of the marble sensors.
ISR(PCINT1_vect)
{
  if (digitalRead(ballPin1) == LOW)
  {
    // set LEDs for left path
    digitalWrite(leftIndicator, HIGH);
    digitalWrite(rightIndicator, LOW);
    // set motors to run and current time.
    ballTimeStamp1 = millis();
    runMotor1 = true;
    reset1 = false;

  }
  if (digitalRead(ballPin2) == LOW)
  {
    // set LEDs for right path
    digitalWrite(leftIndicator, LOW);
    digitalWrite(rightIndicator, HIGH);
    // set motors to run and current time.
    ballTimeStamp2 = millis();
    runMotor2 = true;
    reset2 = false;
  }
}

// Retrace path ISR
void leftRetracePath()
{
  runMotor1 = false;
}

void rightRetracePath()
{
  runMotor2 = false;
}

/*  Generic debouncing function
    Debounces reed switch input by ignoring input after the motor has activated
    and before it has reached a certain time in ms.
*/
void debounceReedInput(bool runMotor, unsigned long ballTimeStamp, bool retraceLight, 
                       LinkedList<Ball> ballQueue, bool reset) 
  {               
  if (runMotor == false) {
    if (millis() - (ballTimeStamp + HOLD_TIME) < MOTOR_RUN_TIME) {
      runMotor = true;
    }
    else if (reset == false) {
      retraceLight = true;
      Ball bl = {millis()};
      ballQueue.add(bl);
      reset = true;
    }
  }
}

// Motor function
void motor(int in1, int in2, bool runMotor)
{
  if (runMotor == true)
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

/*  Generic function for determining whether to run LEDs based on flags.
    When a marble is about to move down the retrace, the LEDs will turn 
    green for 200ms.
    Otherwise the LEDs are red.
*/
void runRetrace(bool retraceLight, LinkedList<Ball> ballQueue, char path)
{
  // get the marble from head of the queue
  Ball bl = ballQueue.get(0);
  if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME)
  {
    // Check if LEDs have been on for at least 200ms. If so, turn to red.
    if (millis() - bl.reedSwitchTimestamp >= RETRACE_TIME + 200)
    {
      ballQueue.shift();
      if (path == 0)
        leftPathLED(255, 0, 0);
      else
        rightPathLED(255, 0, 0);
    }
    else
    {
      if (path == 0)
        leftPathLED(255, 0, 0);
      else
        rightPathLED(255, 0, 0);
      retraceLight = false;
      ballsInSpiralFirstHalf.add(bl);
    }
  }
}

/*  Functions relating to the operation of LEDs.
    The order of LED strips is spiral -> left retrace -> right retrace.
*/
void spiralLED(char loc)
{
  randomColour();
  if (loc == 0)
  {
    for (int i = 0; i < startSpiralStrip; i++)
    {
      pixels.setPixelColor(i, pixels.Color(red, green, blue));
      pixels.show();
    }
  }
  else
  {
    for (int i = startSpiralStrip; i < startSpiralStrip + endSpiralStrip; i++)
    {
      pixels.setPixelColor(i, pixels.Color(red, green, blue));
      pixels.show();
    }
  }
}

void leftPathLED(int r, int g, int b)
{
  for (int i = startSpiralStrip + endSpiralStrip; 
       i < leftRetraceNumStripPixels + startSpiralStrip + endSpiralStrip; i++)
  {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
}

void rightPathLED(int r, int g, int b)
{
  for (int i = leftRetraceNumStripPixels + startSpiralStrip + endSpiralStrip;
       i < rightRetraceNumStripPixels + leftRetraceNumStripPixels + startSpiralStrip + endSpiralStrip; i++)
  {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
}

void randomColour()
{
  red = random(0, 255);
  green = random(0, 255);
  blue = random(0, 255);
}