#include <string.h>
#include <stdlib.h>
#include <Servo.h>

///////////////////////////////////////////////////
// Defines and Constants
///////////////////////////////////////////////////

const int MOTOR_CONTROL_SPEED = 255;
#define MOTOR_TRAVEL_SPEED 0.024
#define MOTOR_ROTATION_SPEED 10.98
#define MAZE_DISTANCE 40
#define TURNING_DISTANCE 10 // TODO: test on the day
#define STAIR_DISTANCE 20 // TODO: test on the day


#define MAX_ARGS 16

#define MOVE_COMMAND "move" // moves certain distance
#define TURN_COMMAND "turn" // turns a direction
#define ROTATE_COMMAND "rotate" // rotates by given angle
#define DRIVE_COMMAND "drive" // drives in given direction until stopped
#define STOP_COMMAND "stop" // stops motors

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

const int IN_CLAW = 100;
const int OUT_CLAW = 15;
const int MAX_PIVOT_ANGLE = 150; 
const int MIN_PIVOT_ANGLE = 90; 

///////////////////////////////////////////////////
// Globals and Pins
///////////////////////////////////////////////////

// Ultrasonic Sensor
const int leftTrig = 2;
const int leftEcho = 4;

const int forwardTrig = 12;
const int forwardEcho = 13;

const int rightTrig = 7;
const int rightEcho = 8;

// Motors
const int leftMotorPin1 = 11;    
const int leftMotorPin2 = 10;
const int rightMotorPin1 = 5;    
const int rightMotorPin2 = 6;  

// Servo
const int clawServoPin = 9;
Servo clawServo;

const int pivotServoPin = 3;
Servo pivotServo;

int currentPivotAngle = MIN_PIVOT_ANGLE;
int currentServoAngle = IN_CLAW;

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
  // checkUltrasonic();
  delay(500);

  if (Serial.available()) {
    char input[128];
    memset(input, '\0', 128);
    readString(input);

    // Read arguments
    char *args[MAX_ARGS];
    memset(args, 0, sizeof(char *) * MAX_ARGS);

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
          currentServoAngle = IN_CLAW;
        } else if (*args[1] == 't') {
          currentServoAngle += 10;
        } else if (*args[1] == 'l') {
          currentServoAngle -= 10;
        }

        clawServo.write(currentServoAngle);
      }
    }
    else if (checkCommand(PIVOT_COMMAND)) {
      if (args[1] != nullptr) {
        if (*args[1] == 'u') {
          movePivotAngle(MAX_PIVOT_ANGLE);
        } else if (*args[1] == 'd') {
          movePivotAngle(0);
        } else if (strcmp(args[1], "lu") == 0) {
          movePivotAngle(currentPivotAngle + 10);
        } else if (strcmp(args[1], "ld") == 0) {
          movePivotAngle(currentPivotAngle - 10);
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
    else if (checkCommand(HELP_COMMAND)) {
      help();
    }
    // If the user provides an invalid command
    else {
      Serial.print("Invalid command ");
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
  clawServo.write(currentServoAngle);

  pivotServo.attach(pivotServoPin);
  pivotServo.write(currentPivotAngle);
  Serial.println("Servo Pins initialized");
}

///////////////////////////////////////////////////
// Helper Functions
///////////////////////////////////////////////////

void moveDistance(int distance) {
  char direction;

  if (distance >= 0) {
    direction = 'f';
    Serial.println("Moving forward ");
  } else {
    direction = 'b';
    Serial.print("Moving backward ");
    distance *= -1;
  }

  float time = distance * 10 / MOTOR_TRAVEL_SPEED ;
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
    if (c == '\n') {
      string[i] = '\0';
    } else {
      string[i] = c;
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
  double time = 0;

  if (angle >= 0) {
    direction = 'r';
    Serial.println("Rotating right");
  } else {
    direction = 'l';
    Serial.println("Rotating left");
    angle *= -1;
  }

  time = angle / MOTOR_ROTATION_SPEED * 1000;

  driveSeconds(time, direction);
}

void turnDirection(char direction) {
  if (direction == 'l') {
    rotateAngle(-90);
  } else if (direction == 'r') {
    rotateAngle(90);
  }
}

void movePivotAngle(int angle) {
  if (angle < MIN_PIVOT_ANGLE) {
    angle = MIN_PIVOT_ANGLE;
  } else if (angle > MAX_PIVOT_ANGLE) {
    angle = MAX_PIVOT_ANGLE;
  }

  currentPivotAngle = angle;
  pivotServo.write(angle);
}

void clawGrab() {
  movePivotAngle(MIN_PIVOT_ANGLE);

  currentServoAngle = OUT_CLAW;
  clawServo.write(currentServoAngle);

  movePivotAngle(MAX_PIVOT_ANGLE);
}

void clawDrop() {
  movePivotAngle(MIN_PIVOT_ANGLE);

  currentServoAngle = IN_CLAW;
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
    Serial.println("\tl - left");
    Serial.println("\tr - right");
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

