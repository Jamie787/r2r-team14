#include <string.h>
#include <stdlib.h>
#include <Servo.h>

///////////////////////////////////////////////////
// Defines and Constants
///////////////////////////////////////////////////

const int MOTOR_CONTROL_SPEED = 100;
#define MOTOR_TRAVEL_SPEED 1
#define MOTOR_ROTATION_SPEED 1
#define MAZE_DISTANCE 42
#define TURNING_DISTANCE 10 // to move 90 degrees (will have to test on the day)
#define STAIR_DISTANCE 20 // (will have to test on the day)


#define MAX_ARGS 16

#define MOVE_COMMAND "move"
#define ROTATE_COMMAND "rotate"
#define DRIVE_COMMAND "drive"
#define STOP_COMMAND "stop"

#define SENSOR_COMMAND "sensor"
#define CLAMP_COMMAND "clamp"
#define PIVOT_COMMAND "pivot"

// Automation Constants
#define SENSE_COMMAND "sense"
#define MAZE_COMMAND "maze"
#define STAIRS_COMMAND "stairs"

#define checkCommand(command) strcmp(args[0], command) == 0

const int IN_CLAW = 110;
const int OUT_CLAW = 0;
const int SERVO_STOP = 90;
const int PIVOT_SPEED = 10; // m/s
const int MAX_PIVOT_DISTANCE = 50; // TODO: figure out what are max is in mm

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

const int pivotServoPin = 3;
Servo pivotServo;

int currentDistance = 0;

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
      if (args[1] != nullptr && args[2] != nullptr) {
        int distance = atoi(args[1]); // convert first arg to integer
        char direction = args[2][0];  // get first character of the second arg

        if (direction == 'f' || direction == 'b' || direction == 'l' || direction == 'r') {
          moveDistance(distance, direction);
        } else {
          Serial.println("Invalid direction. Use 'f', 'b', 'l', or 'r'.");
        }
      } else {
        Serial.println("Invalid command. Please provide both distance and direction.");
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
      if (args[1] != nullptr) {
        if (*args[1] == 'O') {
          
        }
      }
    }
    else if (checkCommand(PIVOT_COMMAND)) {
      if (args[1] != nullptr) {
        if (*args[1] == 'U') {
          movePivotDistance(MAX_PIVOT_DISTANCE);
        } else if (*args[1] == 'D') {
          movePivotDistance(0);
        } else if (strcmp(args[1], "LU") === 0) {
          movePivotDistance(currentDistance + 10);
        } else if (strcmp(args[1], "LD") === 0) {
          movePivotDistance(currentDistance - 10);
        }
      }
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
  clawServo.write(IN_CLAW);

  pivotServo.attach(pivotServoPin);
  pivotServo.write(SERVO_STOP);
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

  int time = distance * 10 / MOTOR_TRAVEL_SPEED ;
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

void moveDistanceOne(int distance, char direction) {
  int time = distance / MOTOR_TRAVEL_SPEED / 100;
  driveSeconds(time, direction);
}

void driveSeconds(int time, char direction) {
  drive(direction);
  delay(time);
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

  int time = angle * 1000 / MOTOR_ROTATION_SPEED;

  driveSeconds(time, direction);
}

void movePivotDistance(int distance) {
  if (distance < 0) {
    distance = 0;
  } else if (distance > MAX_PIVOT_DISTANCE) {
    distance = MAX_PIVOT_DISTANCE;
  }

  int deltaDistance = distance - currentDistance;

  int time = deltaDistance / PIVOT_SPEED;

  if (time > 0) {
    pivotServo.write(SERVO_STOP + 5 + PIVOT_SPEED);
    delay(time);
    pivotServo.write(SERVO_STOP);
  } else if (time < 0) {
    time *= -1;
    pivotServo.write(SERVO_STOP - PIVOT_SPEED);
    delay(time);
    pivotServo.write(SERVO_STOP);
  }
}

void turnDirection(char direction) {
  if (direction == 'l') {
    rotateAngle(-90);
  } else if (direction == 'r') {
    rotateAngle(90);
  }
}