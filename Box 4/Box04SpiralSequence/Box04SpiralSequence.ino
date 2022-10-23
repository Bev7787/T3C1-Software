#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

int redColour = 0;
int greenColour = 0;
int blueColour = 0;

int pathSwitchLeftIndicatorLightPin = 3;
int pathSwitchRightIndicatorLightPin = 2;

//----------------------------\\

Servo topServo;
int topServoPin = 8;
int topServoPos = 1;

int angle1 = 122;
int angle2 = 72;

int topSensorPin  = 12;

int spiralStripPin = 5;
int spiralNumStripPixels = 32;
int spiralLightSequence = 0;

volatile bool changeStripColour = false;

Adafruit_NeoPixel spiralStrip(spiralNumStripPixels , spiralStripPin , NEO_GRB + NEO_KHZ800);

//----------------------------------------------------------------------------------------------------------------------------------------\\

void setup()
{
	unsigned long currentTime = millis();

	digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
	digitalWrite(pathSwitchRightIndicatorLightPin, LOW);

	spiralStrip.begin();
	changeTopStripColour();

	topServo.attach(topServoPin);
	topServo.write(angle1);

	pinMode(topSensorPin, INPUT);
  digitalWrite(topSensorPin, HIGH);

	PCICR |= B00000001;
	PCMSK0 |= B00010000;
}

void loop()
{
	if(changeStripColour == true){
		setColour();
		changeTopStripColour();
		changeStripColour = false;
	}
}

/*
	The purpose of this function is to detect any changes in state
    of the input pins (sensor pins) and set the corresponding bool
    variables to true and record current time (where applicable)
*/
ISR(PCINT0_vect)
{
	if (digitalRead(topSensorPin) == LOW)
	{
		changeStripColour = true;
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
		topServo.write(angle1);
		digitalWrite(pathSwitchRightIndicatorLightPin, LOW);  
		digitalWrite(pathSwitchLeftIndicatorLightPin, HIGH);
	}
	if(topServoPos % 2 != 0)
	{
		topServo.write(angle2);
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