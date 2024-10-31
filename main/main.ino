#include <string.h>
#include <stdlib.h>
#include <Servo.h>

///////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////

#define MAX_ARGS 16

#define MOVE_COMMAND "move"
#define ROTATE_COMMAND "rotate"
#define SENSOR_COMMAND "sensor"
#define DRIVE_COMMAND "drive"
#define CLAMP_COMMAND "clamp"

#define checkCommand(command) strcmp(args[0], command) == 0

///////////////////////////////////////////////////
// Globals and Constants
///////////////////////////////////////////////////

// Ultrasonic Sensor
const int leftTrig = 2;
const int leftEcho = 4;

const int forwardTrig = 0;
const int forwardEcho = 1;

const int rightTrig = 7;
const int rightEcho = 8;

// Motors
const int leftMotorPin1 = 5;    
const int leftMotorPin2 = 6;
const int rightMotorPin1 = 10;    
const int rightMotorPin2 = 11;  

// Servo
const int clawServoPin = 9;
Servo clawServo;

// Not ceratain about this...
// const int pivotServoPin = 3;
// Servo pivotServo;

///////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);

  // TODO: Add pinMode() for all of the pins once they're finalised
  initialiseUltrasonic();
  initaliseMotors();
  initialiseServo();
}

void loop() {
  if (Serial.available()) {
    char input[128];
    memset(input, '\0', 128);
    readString(input);

    // Read arguments
    char *args[MAX_ARGS];
    args[0] = strtok(input, " ");

    for (int i = 1; (args[i] = strtok(nullptr, " ")) != nullptr; i++);


    // Commands
    if (checkCommand(MOVE_COMMAND)) {
      if (args[1] != nullptr) {
        int distance = atoi(args[1]);
        moveDistance(distance);
      } else {
        Serial.println("Invalid command. No distance");
      }
    } 
    else if (checkCommand(SENSOR_COMMAND)) {
      checkUltrasonic();
    }
    else if (checkCommand(ROTATE_COMMAND)) {
    
    }
    else if (checkCommand(DRIVE_COMMAND)) {
      
    }
    else if (checkCommand(CLAMP_COMMAND)) {
      if (args[1] != nullptr) {
        int angle = atoi(args[1]);
        moveClaw(angle);
      }
    }
    // If the user provides an invalid command
    else {
      Serial.println("Invalid command");
      Serial.println(args[0]);
    }
  }
}

// Subroutine
void initialiseUltrasonic() {
  pinMode(leftTrig, OUTPUT);
  pinMode(leftEcho, INPUT);
  pinMode(forwardTrig, OUTPUT);
  pinMode(forwardEcho, INPUT);
  pinMode(rightTrig, OUTPUT);
  pinMode(rightEcho, INPUT);

  Serial.println("Ultrasonic Pins initialized");
}

void initializePins() {
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  
  Serial.println("Motor Pins initialized");
}

void initialiseServo() {
  clawServo.attach(clawServoPin);
  clawServo.write(0);

  // TODO: Figure out the difference between 180 and 360
  // pivotServo.attach(pivotServoPin);
  // pivotServo.write(0);
  Serial.println("Servo Pins initialized");
}

// Helper Functions
void moveDistance(int distance) {
  if (distance >= 0) {
    Serial.print("Moving forward ");
  } else {
    Serial.print("Moving backward ");
    distance *= -1;
  }

  Serial.print(distance);
  Serial.println(" cm");
}

void checkUltrasonic() {
  float leftDistance = findDistance(leftTrig, leftEcho);
  float forwardDistance = findDistance(forwardTrig, forwardEcho);
  float rightDistance = findDistance(rightTrig, rightEcho);

  Serial.print("\t\tForward distance: ");
  Serial.println(forwardDistance);

  Serial.print("Left distance: ");
  Serial.print(leftDistance);
  Serial.print("\t\t Right distance: ");
  Serial.println(rightDistance);
}

float findDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration / 2 * 0.034;

  return distance;
}

void readString(char *string) {
  delay(100);

  int i;
  for (i = 0; Serial.available(); i++) {
    char c = Serial.read();
    if (c != '\n') {
      string[i]  = c;
      i--;
    }
  }

  string[i] = '\0';
}

void moveClaw(int angle) {
  if (angle >= 0 && angle <= 180) {
    Serial.print("Clamp at ");
    Serial.print(angle);
    Serial.println(" degrees");

    clawServo.write(angle);
  } else {
    Serial.println("Invalid angle");
  }
}

void initaliseMotors() {
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  Serial.println("Motors initialised");
}