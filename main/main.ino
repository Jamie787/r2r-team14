#include <string.h>
#include <stdlib.h>
#include <Servo.h>

///////////////////////////////////////////////////
// Defines and Constants
///////////////////////////////////////////////////

const int MOTOR_CONTROL_SPEED = 100;
#define MOTOR_TRAVEL_SPEED 1
#define MOTOR_ROTATION_SPEED 1

#define MAX_ARGS 16

#define MOVE_COMMAND "move"
#define ROTATE_COMMAND "rotate"
#define DRIVE_COMMAND "drive"
#define STOP_COMMAND "stop"

#define SENSOR_COMMAND "sensor"
#define CLAMP_COMMAND "clamp"

// Automation Constants
#define SENSE_COMMAND "sense"
#define MAZE_COMMAND "maze"
#define STAIRS_COMMAND "stairs"

#define checkCommand(command) strcmp(args[0], command) == 0

///////////////////////////////////////////////////
// Globals and Pins
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

// Not certain about this...
// const int pivotServoPin = 3;
// Servo pivotServo;

///////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);

  // TODO: Add pinMode() for all of the pins once they're finalised
  initialiseUltrasonic();
  initialiseMotors();
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
    if (args[0] == nullptr) {
      Serial.println("No command");
      return;
    }

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
      if (args[1] != nullptr) {
        int angle = atoi(args[1]);
        rotateAngle(angle);
      } else {
        Serial.println("Invalid command. No angle");
      }
    }
    else if (checkCommand(DRIVE_COMMAND)) {
      if (args[1] == nullptr) {
        Serial.println("Invalid drive command, no direction");
      }

      char direction = *args[1];

      if (!(direction == 'f' || direction == 'b' || direction == 'l' || direction == 'r')) {
        Serial.print("Invalid drive command, '");
        Serial.print(direction);
        Serial.println("' is an invalid direction");
      }

      drive(direction);
    }
    else if (checkCommand(STOP_COMMAND)) {
      halt();
    }
    else if (checkCommand(CLAMP_COMMAND)) {
      // if (args[1] != nullptr) {
      //   int angle = atoi(args[1]);
      //   moveClaw(angle);
      // }
    }
    // If the user provides an invalid command
    else {
      Serial.println("Invalid command");
      Serial.println(args[0]);
    }
  }
}

///////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////

void initialiseUltrasonic() {
  pinMode(leftTrig, OUTPUT);
  pinMode(leftEcho, INPUT);
  pinMode(forwardTrig, OUTPUT);
  pinMode(forwardEcho, INPUT);
  pinMode(rightTrig, OUTPUT);
  pinMode(rightEcho, INPUT);

  Serial.println("Ultrasonic Pins initialized");
}

void initialiseMotors() {
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

///////////////////////////////////////////////////
// Helper Functions
///////////////////////////////////////////////////

void moveDistance(int distance) {
  char direction;

  if (distance >= 0) {
    direction = 'f';
    Serial.print("Moving forward ");
  } else {
    direction = 'b';
    Serial.print("Moving backward ");
    distance *= -1;
  }

  int time = distance / MOTOR_TRAVEL_SPEED / 100;
  driveSeconds(time, direction);
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

void driveSeconds(int time, char direction) {
  drive(direction);
  delay(time * 1000);
  halt();
}

void drive(char direction) {
  if (direction == 'f') {
    analogWrite(leftMotorPin1, 0);
    analogWrite(leftMotorPin2, MOTOR_CONTROL_SPEED);
    analogWrite(rightMotorPin1, 0);
    analogWrite(rightMotorPin2, MOTOR_CONTROL_SPEED);
  }
  else if (direction == 'b') {
    analogWrite(leftMotorPin1, MOTOR_CONTROL_SPEED);
    analogWrite(leftMotorPin2, 0);
    analogWrite(rightMotorPin1, MOTOR_CONTROL_SPEED);
    analogWrite(rightMotorPin2, 0);
  }
  else if (direction == 'l') {
    analogWrite(leftMotorPin1, MOTOR_CONTROL_SPEED);
    analogWrite(leftMotorPin2, 0);
    analogWrite(rightMotorPin1, 0);
    analogWrite(rightMotorPin2, MOTOR_CONTROL_SPEED);
  }
  else if (direction == 'r') {
    analogWrite(leftMotorPin1, 0);
    analogWrite(leftMotorPin2, MOTOR_CONTROL_SPEED);
    analogWrite(rightMotorPin1, MOTOR_CONTROL_SPEED);
    analogWrite(rightMotorPin2, 0);
  }
}

void halt() {
  analogWrite(leftMotorPin1, 0);
  analogWrite(leftMotorPin2, 0);
  analogWrite(rightMotorPin1, 0);
  analogWrite(rightMotorPin2, 0);
}

void rotateAngle(int angle) {
  char direction;

  if (angle >= 0) {
    direction = 'r';
    Serial.println("Rotating right");
  } else {
    direction = 'l';
    Serial.println("Rotating left");
    angle *= -1;
  }

  int time = angle / MOTOR_ROTATION_SPEED;

  driveSeconds(time, direction);
}