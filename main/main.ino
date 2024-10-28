#include <string.h>
#include <stdlib.h>
// TEST 3
// Defines

#define MAX_ARGS 16

#define MOVE_COMMAND "move"
#define ROTATE_COMMAND "rotate"
#define SENSOR_COMMAND "sensor"
#define DRIVE_COMMAND "drive"

// Globals and Constants

// Ultrasonic
const int leftTrig = -1;
const int leftEcho = -1;

const int forwardTrig = -1;
const int forwardEcho = -1;

const int rightTrig = -1;
const int rightEcho = -1;


// Servo
const int clawServo = -1;
const int pivotServo = -1;


// Motors
const int leftMotor = -1;
const int rightMotor = -1;


// Functions

void setup() {
  Serial.begin(9600);

  // TODO: Add pinMode() for all of the pins once they're finalised
}

void loop() {
  if (Serial.available()) {
    // Reads a line from the user and converts it to a C style string
    // The string object should not be used as strtok() ruins the array
    String inputString = Serial.readString();
    char *input = const_cast<char *>(inputString.c_str());

    // Read arguments
    char *args[MAX_ARGS];
    args[0] = strtok(input, " ");

    for (int i = 1; (args[i] = strtok(nullptr, " ")) != nullptr; i++);


    // Commands
    if (strcmp(args[0], MOVE_COMMAND) == 0) {
      int distance = atoi(args[1]);

      moveDistance(distance);
    } 
    else if (strcmp(args[0], SENSOR_COMMAND) == 0) {
      char sensorType = *args[1];

      if (sensorType == 'u') {
        checkUltrasonic();
      } else {
        Serial.println("Invalid Sensor command");
      }
    }
    else if (strcmp(args[0], ROTATE_COMMAND) == 0) {
    
    }
    else if (strcmp(args[0], DRIVE_COMMAND) == 0) {
    
    }
    // If the user provides an invalid command
    else {
      Serial.println("Invalid command");
    }
  }
}

void moveDistance(int distance) {
  if (distance >= 0) {
    Serial.print("Moving forward ");
  } else {
    Serial.print("Moving backward ");
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
