#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

unsigned long startLeftMillis;
unsigned long startRightMillis;

//--------------------------------------------\\

Servo leftRetraceServo;
int leftRetraceServoPin = 10;
int leftRetraceServoPos = 0;

int leftSensorState = 1;
int leftSensorLastState = 0;
int leftSensorPin = 11;

int leftRetraceStripPin = 9;
int leftRetraceNumStripPixels = 10;

bool leftBallStationary = false;
bool runLeftRetrace = false;
bool rotateLeftServo = false;

//--------------------------------------------\\

Servo rightRetraceServo;
int rightRetraceServoPin = 7;
int rightRetraceServoPos = 0;

int rightSensorState = 1;
int rightSensorLastState = 0;
int rightSensorPin = 13;

int rightRetraceStripPin = 6;
int rightRetraceNumStripPixels = 10;

bool rightBallStationary = false;
bool runRightRetrace = false;
bool rotateRightServo = false;

//--------------------------------------------\\

Adafruit_NeoPixel leftRetraceStrip(leftRetraceNumStripPixels, leftRetraceStripPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRetraceStrip(rightRetraceNumStripPixels, rightRetraceStripPin, NEO_GRB + NEO_KHZ800);

//--------------------------------------------------------------------------------------------------------------\\

void setup(){
	Serial.begin(9600);
	unsigned long currentTime = millis();
	leftRetraceServo.attach (leftRetraceServoPin);
	leftRetraceServo.write(0);

	leftRetraceStrip.begin();
	leftRetraceRedColour();

	rightRetraceServo.attach (rightRetraceServoPin);
	rightRetraceServo.write(0);

	rightRetraceStrip.begin();
	rightRetraceRedColour();

	pinMode(leftSensorPin, INPUT);
    pinMode(rightSensorPin, INPUT);

	PCICR |= B00000001;
	PCMSK0 |= B00101000;
}

void loop(){
	if (runLeftRetrace == true){
		if(leftBallStationary == true){
			leftStationaryBall();
			leftBallStationary = false;
		}

		if(millis() - startLeftMillis >= 1000 && rotateLeftServo == true){
			leftReleaseBall();
			rotateLeftServo = false;
		}

		if(millis() - startLeftMillis >= 3000){ //bufefr
			leftRetraceRedColour();
			runLeftRetrace = false;
		}
	}

	if (runRightRetrace == true){
		if(rightBallStationary == true){
			rightStationaryBall();
			rightBallStationary = false;
		}

		if(millis() - startRightMillis >= 1000 && rotateRightServo == true){
			rightReleaseBall();
			rotateRightServo = false;
		}

		if(millis() - startRightMillis >= 3000){ //bufefr
			rightRetraceRedColour();
			runRightRetrace = false;
		}
	}
}

ISR(PCINT0_vect){
	if (digitalRead(leftSensorPin) == HIGH){
		startLeftMillis = millis();
		leftBallStationary = true;
		rotateLeftServo = true;
		runLeftRetrace = true;
	}

	if (digitalRead(rightSensorPin) == HIGH){
		startRightMillis = millis();
		rightBallStationary = true;
		rotateRightServo = true;
		runRightRetrace = true;
	}
}

//--------------------------------------------------------------------------------------------------------------\\

void rightStationaryBall(){
	for (int i = 0; i < rightRetraceNumStripPixels; i++){
		rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 0, 255));
		rightRetraceStrip.show();
	}
}

void rightReleaseBall(){
	for (int i = 0; i < rightRetraceNumStripPixels; i++){
		rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(0, 255, 0));
		rightRetraceStrip.show();
	}
	rightRetraceServo.write(180);
}

void rightRetraceRedColour() {
	for (int i = 0; i < rightRetraceNumStripPixels; i++){
		rightRetraceStrip.setPixelColor(i, rightRetraceStrip.Color(255, 0, 0));
		rightRetraceStrip.show();
	}
	rightRetraceServo.write(0);
}

//--------------------------------------------------------------------------------------------------------------\\

void leftStationaryBall(){
	//Blue LED strip when ball is stationary
	for (int i = 0; i < leftRetraceNumStripPixels; i++){
		leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 0, 255));
		leftRetraceStrip.show();
	}
}

void leftReleaseBall(){
	for (int i = 0; i < leftRetraceNumStripPixels; i++){
		leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(0, 255, 0));
		leftRetraceStrip.show();
	}
	leftRetraceServo.write(180);
}

void leftRetraceRedColour() {
	for (int i = 0; i < leftRetraceNumStripPixels; i++){
		leftRetraceStrip.setPixelColor(i, leftRetraceStrip.Color(255, 0, 0));
		leftRetraceStrip.show();
	}
	leftRetraceServo.write(0);
}
