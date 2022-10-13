#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

volatile unsigned long startLeftMillis;
volatile unsigned long startRightMillis;

int redColour = 0;
int greenColour = 0;
int blueColour = 0;

int pathSwitchLeftIndicatorLightPin = 3;
int pathSwitchRightIndicatorLightPin = 2;

//----------------------------\\

Servo topServo;
int topServoPin = 8;
int topServoPos = 1;

int topSensorPin  = 12;

int spiralStripPin = 5;
int spiralNumStripPixels = 32;
int spiralLightSequence = 0;

volatile bool changeStripColour = false;

//----------------------------\\

Servo leftRetraceServo;
int leftRetraceServoPin = 10;
int leftRetraceServoPos = 0;

int leftSensorPin = 11;

int leftRetraceStripPin = 9;
int leftRetraceNumStripPixels = 10;

volatile bool leftBallStationary = false;
volatile bool runLeftRetrace = false;
volatile bool rotateLeftServo = false;

//----------------------------\\

Servo rightRetraceServo;
int rightRetraceServoPin = 7;
int rightRetraceServoPos = 0;

int rightSensorPin = 13;

int rightRetraceStripPin = 6;
int rightRetraceNumStripPixels = 10;

volatile bool rightBallStationary = false;
volatile bool runRightRetrace = false;
volatile bool rotateRightServo = false;

//----------------------------\\

Adafruit_NeoPixel spiralStrip(spiralNumStripPixels , spiralStripPin , NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leftRetraceStrip(leftRetraceNumStripPixels, leftRetraceStripPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRetraceStrip(rightRetraceNumStripPixels, rightRetraceStripPin, NEO_GRB + NEO_KHZ800);

//----------------------------------------------------------------------------------------------------------------------------------------\\

void setup()
{
	unsigned long currentTime = millis();

	digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
	digitalWrite(pathSwitchRightIndicatorLightPin, LOW);

	spiralStrip.begin();
	changeTopStripColour();

	topServo.attach(topServoPin);
	topServo.write(0);

	leftRetraceServo.attach (leftRetraceServoPin);
	leftRetraceServo.write(30);

	leftRetraceStrip.begin();
	leftRetraceRedColour();

	rightRetraceServo.attach (rightRetraceServoPin);
	rightRetraceServo.write(30);

	rightRetraceStrip.begin();
	rightRetraceRedColour();

	pinMode(leftSensorPin, INPUT);
	pinMode(rightSensorPin, INPUT);
	pinMode(topSensorPin, INPUT);

	PCICR |= B00000001;
	PCMSK0 |= B00111000;
}

void loop()
{
	if(changeStripColour == true){
		setColour();
		changeTopStripColour();
		changeStripColour = false;
	}

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

		if(millis() - startLeftMillis >= 3000)
		{
			leftRetraceRedColour();
			runLeftRetrace = false;
		}
	}

	if (runRightRetrace == true)
	{
		if(rightBallStationary == true)
		{
			rightStationaryBall();
			rightBallStationary = false;
		}

		if(millis() - startRightMillis >= 1000 && rotateRightServo == true)
		{
			rightReleaseBall();
			rotateRightServo = false;
		}

		if(millis() - startRightMillis >= 3000)
		{
			rightRetraceRedColour();
			runRightRetrace = false;
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
	if (digitalRead(topSensorPin) == HIGH)
	{
		changeStripColour = true;
	}

	if (digitalRead(leftSensorPin) == HIGH)
	{
		startLeftMillis = millis();
		leftBallStationary = true;
		rotateLeftServo = true;
		runLeftRetrace = true;
	}

	if (digitalRead(rightSensorPin) == HIGH)
	{
		startRightMillis = millis();
		rightBallStationary = true;
		rotateRightServo = true;
		runRightRetrace = true;
	}
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

/*
	The purpose of this function is to rotate the top servo to switch the 
    path the ball takes between left path and right path and turn on the
    corresponding led indicator light bulb
*/
void changeTopServo(int topServoPos)
{
	if(topServoPos % 2 == 0)
	{ 
		topServo.write(90);
		digitalWrite(pathSwitchRightIndicatorLightPin, LOW);  
		digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
	}
	if(topServoPos % 2 != 0)
	{
		topServo.write(0);
		digitalWrite(pathSwitchLeftIndicatorLightPin, LOW);
		digitalWrite(pathSwitchRightIndicatorLightPin, HIGH);
	}
}

/*
	The purpose of this function is to change the colour of the top spiral light
    and call the function to rotate the top servo
*/
void changeTopStripColour() 
{
	setColour();
	if (spiralLightSequence == 0) 
	{
		changeTopServo(topServoPos);
		for (int i = 0; i < spiralNumStripPixels ; i++) 
		{
			spiralStrip.setPixelColor(i, spiralStrip.Color(redColour, greenColour, blueColour));
			spiralStrip.show();
		}
		spiralLightSequence++;
		topServoPos++;
	}

	else if (spiralLightSequence == 1) 
	{
		changeTopServo(topServoPos);
		for (int i = 0; i < spiralNumStripPixels ; i++) 
		{
			spiralStrip.setPixelColor(i, spiralStrip.Color(redColour, greenColour, blueColour));
			spiralStrip.show();
		}
		spiralLightSequence++;
		topServoPos++;
	}

	else 
	{
		changeTopServo(topServoPos);
		for (int i = 0; i < spiralNumStripPixels ; i++) 
		{
			spiralStrip.setPixelColor(i, spiralStrip.Color(redColour, greenColour, blueColour));
			spiralStrip.show();
		}
		spiralLightSequence = 0;
		topServoPos++;
	}
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
	leftRetraceServo.write(101);
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
	leftRetraceServo.write(30);
}

//----------------------------------------------------------------------------------------------------------------------------------------\\

/*
	The purpose of this function is to show the marble is stationary by turning the
    retrace light strip to blue 
*/
void rightStationaryBall()
{
	for (int i = 0; i < rightRetraceNumStripPixels; i++)
	{
		rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 0, 255));
		rightRetraceStrip.show();
	}
}

/*
	The purpose of this function is to show that the ball cannot pass through by turning
    the light strip to red and rotating the servo up
*/
void rightReleaseBall()
{
	for (int i = 0; i < rightRetraceNumStripPixels; i++)
	{
		rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 255, 0));
		rightRetraceStrip.show();
	}
	rightRetraceServo.write(101);
}

/*
	The purpose of this function is to show that the ball cannot pass through by turning
    the light strip to red and rotating the servo up
*/
void rightRetraceRedColour() 
{
	for (int i = 0; i < rightRetraceNumStripPixels; i++)
	{
		rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(255, 0, 0));
		rightRetraceStrip.show();
	}
	rightRetraceServo.write(30);
}