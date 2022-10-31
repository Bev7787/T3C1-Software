// C++ code for box 3.
#include <Adafruit_NeoPixel.h>
#include <QList.h>

// Timers for LED operation.
// Defined from IR Sensor detection to start of component.
unsigned long HOLD_TIME = 1000;
unsigned long MOTOR_RUN_TIME = 400;
unsigned long RETRACE_TIME = HOLD_TIME + 2 * MOTOR_RUN_TIME + 0; // Placeholder value
unsigned long SPIRAL_TIME = 500;  // Placeholder value
unsigned long FIRST_HALF_SPIRAL = 500;


// Timestamps utilised from 
volatile unsigned long ballTimeStamp1 = 0;
volatile unsigned long ballTimeStamp2 = 0;

volatile bool runMotor1 = false;
volatile bool runMotor2 = false;
volatile bool *m1 = &runMotor1;
volatile bool *m2 = &runMotor2;

volatile bool retraceLightLeft = false;
volatile bool retraceLightRight = false;
volatile bool *rl = &retraceLightLeft;
volatile bool *rr = &retraceLightRight;

// LED strip configuration
int stripPin = A2;
int leftRetraceNumStripPixels = 3;
int rightRetraceNumStripPixels = 3;
int startSpiralStrip = 10;
int endSpiralStrip = 9;
Adafruit_NeoPixel pixels(leftRetraceNumStripPixels + rightRetraceNumStripPixels + startSpiralStrip + endSpiralStrip,
                         stripPin, NEO_GRB + NEO_KHZ800);
                         
// Indicator LED pins
int leftIndicator = A3;
int rightIndicator = A4;

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
  unsigned long timestamp;
  int r;
  int g;
  int b;
};

QList<Ball> leftRetraceQueue;
QList<Ball> rightRetraceQueue;
QList<Ball> ballsInSpiralFirstHalf; // spiral path queue first half
QList<Ball> ballsInSpiralSecondHalf; // spiral path queue second half

void setup()
{
  Serial.begin(115200);
  // Set sensors
  pinMode(motorSen1, INPUT_PULLUP);
  pinMode(motorSen2, INPUT_PULLUP);

  // Set LEDs
  pinMode(leftIndicator, OUTPUT);
  pinMode(rightIndicator, OUTPUT);

  pixels.begin();
  pixels.setBrightness(40);
  spiralSetup();
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
  analogWrite(enA, 75);
  analogWrite(enB, 75);

  // Motor off on initialisation.
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  // Turns LED off due to genius wiring.
  digitalWrite(leftIndicator, HIGH);
  digitalWrite(rightIndicator, HIGH);


  // Attach reed switch interrupts
  attachInterrupt(digitalPinToInterrupt(motorSen1), leftRetracePath, FALLING);
  attachInterrupt(digitalPinToInterrupt(motorSen2), rightRetracePath, FALLING);
}

void loop()
{
  Serial.print("ball 1: ");
  Serial.println(ballTimeStamp1);
  Serial.print("ball 2: ");
  Serial.println(ballTimeStamp2);
  
  // Run motors after allowing the marble to remain stationary for at least 1 second.
  motor(in1, in2, m1, ballTimeStamp1);
  motor(in3, in4, m2, ballTimeStamp2);

  // Run the LEDs on the retrace ramps.
  if (retraceLightLeft)
    runRetrace(rl, ballTimeStamp1, 0);

  if (retraceLightRight)
    runRetrace(rr, ballTimeStamp2, 1);

  // Spiral LED. Runs if there are elements in spiral queue.
  if (ballsInSpiralFirstHalf.length() > 0)
  {
    Ball bl = ballsInSpiralFirstHalf.get(0);
    if (millis() - bl.timestamp >= RETRACE_TIME + FIRST_HALF_SPIRAL)
    {
      spiralLED(bl, 0);
      ballsInSpiralFirstHalf.pop_front();
      ballsInSpiralSecondHalf.push_back(bl);
    }
  }

  if (ballsInSpiralSecondHalf.length() > 0)
  {
    Ball bl = ballsInSpiralSecondHalf.get(0);
    if (millis() - bl.timestamp >= RETRACE_TIME + FIRST_HALF_SPIRAL + SPIRAL_TIME)
    {
      spiralLED(bl, 1);
      ballsInSpiralSecondHalf.pop_front();
    }
  }
}

// Retrace path ISR
void leftRetracePath()
{
  if (runMotor1 == false) {
    ballTimeStamp1 = millis();
    runMotor1 = true;
    digitalWrite(leftIndicator, LOW);
    digitalWrite(rightIndicator, HIGH); 
    retraceLightLeft = true;
    Ball bl = {millis(), randomColour(), randomColour(), randomColour()};
    leftRetraceQueue.push_back(bl);    
  }
}

void rightRetracePath()
{
  if (runMotor2 == false) {
    ballTimeStamp2 = millis();
    runMotor2 = true;
    digitalWrite(leftIndicator, HIGH);
    digitalWrite(rightIndicator, LOW); 
    retraceLightRight = true;
    Ball bl = {millis(), randomColour(), randomColour(), randomColour()};
    rightRetraceQueue.push_back(bl);  
  }
}

// Motor function
void motor(int in1, int in2, volatile bool *runMotor, volatile unsigned long ballTimeStamp)
{
  if (*runMotor == true)
  {
    if ((millis() - ballTimeStamp) >= HOLD_TIME + 2 * MOTOR_RUN_TIME)
    {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      *runMotor = false;
    }
      
    else if ((millis() - ballTimeStamp) >= HOLD_TIME + MOTOR_RUN_TIME)
    {
      // Turn off motors.
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
    else if ((millis() - ballTimeStamp) >= HOLD_TIME)
    {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
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
void runRetrace(bool *retraceLight, unsigned long time, char path)
{
  Ball bl;  
  if (path == 0)
    bl = leftRetraceQueue.get(0);
  else
    bl = rightRetraceQueue.get(0);
    
  // get the marble from head of the queue
  if (millis() - bl.timestamp >= RETRACE_TIME)
  {
    // Check if LEDs have been on for at least 200ms. If so, turn to red.
    if (millis() - bl.timestamp < RETRACE_TIME + 200)
    {
      if (path == 0)
        leftPathLED(0, 255, 0);
      else
        rightPathLED(0, 255, 0);
    }
    else
    {
      if (path == 0)
        leftPathLED(255, 0, 0);
      else
        rightPathLED(255, 0, 0);
      *retraceLight = false;
      ballsInSpiralFirstHalf.push_back(bl);
      if (path == 0)
        leftRetraceQueue.pop_front();
      else
        rightRetraceQueue.pop_front();        
    }
  }
}

/*  Functions relating to the operation of LEDs.
    The order of LED strips is spiral -> left retrace -> right retrace.
*/
void spiralLED(Ball bl, char loc)
{
  if (loc == 0)
  {
    for (int i = endSpiralStrip; i < startSpiralStrip + endSpiralStrip; i++)
    {
      pixels.setPixelColor(i, pixels.Color(bl.r, bl.g, bl.b));
      pixels.show();
    }
  }
  else
  {
    for (int i = 0; i < endSpiralStrip; i++)
    {
      pixels.setPixelColor(i, pixels.Color(bl.r, bl.g, bl.b));
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

// SETUP FUNCTIONS

void spiralSetup()
{
  for (int i = 0; i < startSpiralStrip + endSpiralStrip; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    pixels.show();
  }
}

// Provides values to randomise colours for the spiral LED.
int randomColour()
{
  int n = random(0, 90);
  if (n < 30)
    return 0;
  else if (n < 60)
    return 128;
  return 255;   
}