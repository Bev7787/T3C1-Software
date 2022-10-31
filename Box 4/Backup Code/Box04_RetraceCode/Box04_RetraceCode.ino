#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

volatile unsigned long startLeftMillis;

int redColour = 0;
int greenColour = 0;
int blueColour = 0;

int downPos = 30;
int upPos = 100;

//----------------------------\\

Servo leftRetraceServo;
int leftRetraceServoPin = 10;
int leftRetraceServoPos = 0;

int leftSensorPin = 12;

int leftRetraceStripPin = 9;
int leftRetraceNumStripPixels = 10;

volatile bool leftBallStationary = false;
volatile bool runLeftRetrace = false;
volatile bool rotateLeftServo = false;
volatile bool tempflag = false;

//----------------------------\\

Adafruit_NeoPixel leftRetraceStrip(leftRetraceNumStripPixels, leftRetraceStripPin, NEO_GRB + NEO_KHZ800);

//----------------------------------------------------------------------------------------------------------------------------------------\\

void setup()
{
	unsigned long currentTime = millis();

  // Serial.begin(9600);

	leftRetraceServo.attach (leftRetraceServoPin);
	leftRetraceServo.write(downPos);

	leftRetraceStrip.begin();
	leftRetraceRedColour();

	pinMode(leftSensorPin, INPUT);
  digitalWrite(leftSensorPin, HIGH);

	PCICR |= B00000001;
	PCMSK0 |= B00010000;
}

void loop()
{
  //  Serial.println("Sensor NOT Triggered");
	if (runLeftRetrace == true)
	{
		if(leftBallStationary == true)
		{
			leftStationaryBall();
			leftBallStationary = false;
		}

		if(millis() - startLeftMillis >= 1000 && rotateLeftServo == true)
		{
			leftReleaseBall();
			rotateLeftServo = false;
		}

		if(millis() - startLeftMillis >= 1600 && tempflag == true)
		{
			leftRetraceRedColour();
			runLeftRetrace = false;
      tempflag = false;
		}
	}
}

/*
	The purpose of this function is to detect any changes in state
    of the input pins (sensor pins) and set the corresponding bool
    variables to true and record current time (where applicable)
*/
ISR(PCINT0_vect)
{
	if (digitalRead(leftSensorPin) == LOW)
	{
    // Serial.println("Sensor Triggered");
		startLeftMillis = millis();
		leftBallStationary = true;
		rotateLeftServo = true;
		runLeftRetrace = true;
    tempflag = true;
	}
  // else{
  //   Serial.println("Sensor NOT Triggered");
  // }
}

//----------------------------------------------------------------------------------------------------------------------------------------\\

/*
	The purpose of this function is to randomly generate a new colour for
    the strip lights
*/
void setColour()
{
	redColour = random(0, 255);
	greenColour = random(0,255);
	blueColour = random(0, 255);
}

//----------------------------------------------------------------------------------------------------------------------------------------\\

/*
	The purpose of this function is to show the marble is stationary by turning the
    retrace light strip to blue
*/
void leftStationaryBall()
{
	for (int i = 0; i < leftRetraceNumStripPixels; i++)
	{
		leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 0, 255));
		leftRetraceStrip.show();
	}
}

/*
	The purpose of this function is to show the marble is moving again by turning
    the light strip to green and rotating the servo
*/
void leftReleaseBall()
{
	for (int i = 0; i < leftRetraceNumStripPixels; i++)
	{
		leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 255, 0));
		leftRetraceStrip.show();
	}
  // Serial.println("ROTATE SERVO");
	leftRetraceServo.write(downPos);
}

/*
	The purpose of this function is to show that the ball cannot pass through by turning
    the light strip to red and rotating the servo up
*/
void leftRetraceRedColour() 
{
	for (int i = 0; i < leftRetraceNumStripPixels; i++)
	{
		leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(255, 0, 0));
		leftRetraceStrip.show();
	}
  // Serial.println("ROTATE SERVO");
	leftRetraceServo.write(upPos);
}