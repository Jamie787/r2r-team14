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
#define TURNING_DISTANCE 10 // TODO: test on the day
#define STAIR_DISTANCE 20 // TODO: test on the day


#define MAX_ARGS 16

#define MOVE_COMMAND "move" // moves certain distance
#define TURN_COMMAND "turn" // turns a direction
#define ROTATE_COMMAND "rotate" // rotates by given angle
#define DRIVE_COMMAND "drive" // drives in given direction until stopped
#define STOP_COMMAND "s" // stops motors

#define SENSOR_COMMAND "sensor" // prints distance from all 3 sensors

#define CLAW_COMMAND "claw" // clamps object
#define PIVOT_COMMAND "pivot" // moves claw up and down object

#define HELP_COMMAND "help" // prints help information for all commands

// Automation Constants
#define SENSE_COMMAND "sense"
#define MAZE_COMMAND "maze"
#define STAIRS_COMMAND "stairs"
#define GRAB_COMMAND "grab"
#define DROP_COMMAND "drop"

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
int currentServoAngle = 0;

///////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);

  initialiseUltrasonic();
  initialiseMotors();
  initialiseServo();

  help();

  Serial.println("\nEnter a command:");
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
        Serial.println("Invalid command. Please provide distance");
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
    else if (checkCommand(CLAW_COMMAND)) {
      if (args[1] != nullptr) {
        if (*args[1] == 'o') {
          currentServoAngle = OUT_CLAW;
        } else if (*args[1] == 'i') {
          currentServoAngle = 0;
        } else if (*args[1] == 't') {
          currentServoAngle += 10;
        } else if (*args[1] == 'l') {
          currentServoAngle -= 10;
        }

        clawServo.write(currentDistance);
      }
    }
    else if (checkCommand(PIVOT_COMMAND)) {
      if (args[1] != nullptr) {
        if (*args[1] == 'u') {
          movePivotDistance(MAX_PIVOT_DISTANCE);
        } else if (*args[1] == 'd') {
          movePivotDistance(0);
        } else if (strcmp(args[1], "lu") == 0) {
          movePivotDistance(currentDistance + 10);
        } else if (strcmp(args[1], "ld") == 0) {
          movePivotDistance(currentDistance - 10);
        }
      }
    } 
    else if (checkCommand(MAZE_COMMAND)) {
      moveThroughMaze();
    }
    else if (checkCommand(SENSE_COMMAND)) {
      senseSurroundings();
    } 
    else if (checkCommand(STAIRS_COMMAND)) {
      traverseStairs();
    }
    else if (checkCommand(GRAB_COMMAND)) {
      clawGrab();
    }
    else if (checkCommand(DROP_COMMAND)) {
      clawDrop();
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
  float leftDistance = findSensorDistance(leftTrig, leftEcho);
  float forwardDistance = findSensorDistance(forwardTrig, forwardEcho);
  float rightDistance = findSensorDistance(rightTrig, rightEcho);

  Serial.print("\t\tForward distance: ");
  Serial.println(forwardDistance);

  Serial.print("Left distance: ");
  Serial.print(leftDistance);
  
  Serial.print("\t\t Right distance: ");
  Serial.println(rightDistance);
}

float findSensorDistance(int trigPin, int echoPin) {
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

// void moveRobotDistance(int distance, char direction) {
//   int time = distance / MOTOR_TRAVEL_SPEED / 100;
//   driveSeconds(time, direction);
// }

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

void turnDirection(char direction) {
  if (direction == 'l') {
    rotateAngle(-90);
  } else if (direction == 'r') {
    rotateAngle(90);
  }
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

void clawGrab() {
  movePivotDistance(0);

  currentServoAngle = OUT_CLAW + 15;
  clawServo.write(currentServoAngle);

  movePivotDistance(MAX_PIVOT_DISTANCE);
}

void clawDrop() {
  movePivotDistance(0);

  currentServoAngle = 0;
  clawServo.write(currentServoAngle);
}

void help() {
  Serial.println("Robot Control Menu:");
  
  Serial.println("\nCommands:");

  Serial.println("\nhelp - display's the robot control menu\n");

  Serial.println("Motors:");
  Serial.println("move [distance] - moves forward or backwards by the given distance and sign, with positive being forward");
  Serial.println("turn [l - r] - turns 90 degrees left or right depending on the input");
  Serial.println("rotate [angle] - rotates left or right by a given angle and sign, with positive being right");
  Serial.println("drive [direction] - drives in a given direction");
    Serial.println("\tDirections:");
    Serial.println("\tf - forward");
    Serial.println("\tb - backward");
    Serial.println("\tr - right");
    Serial.println("\tl - left");
  Serial.println("stop - stops motors if they are enabled from drive");

  Serial.println("\nSensors:");
  Serial.println("sensor - prints the distance from the three ultrasonic sensors");

  Serial.println("\nServo: ");
  Serial.println("claw [state] - changes the position of the claw to one of the states");
    Serial.println("\tClaw States:");
    Serial.println("\to - out");
    Serial.println("\ti - in");
    Serial.println("\tt - tighten");
    Serial.println("\tl - loosen");
  Serial.println("pivot [state] - pivots the height of the claw to a given state");
    Serial.println("\tPivot States:");
    Serial.println("\tu - fully up");
    Serial.println("\td - fully down");
    Serial.println("\tlu - little up");
    Serial.println("\tld - little down");

  Serial.println("\nAutomation:");
  Serial.println("sense - check surroundings");
  Serial.println("maze - move through maze");
  Serial.println("stairs - traverse up stairs");
  Serial.println("grab - picks up object");
  Serial.println("drop - drops object");
}

