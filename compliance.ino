// Compliance Testing

// This program:
// - Prints Initializations message
// - Sensors: print the distance of nearby object to Serial Monitor
// - Motors: move forwards, backwards, left, and right depending on speed given as a number between 1-5
// - Servos: move up, down, in, or out

#include <Servo.h>

// Constants
#define EXIT 'x'

#define MOTOR_FORWARD 'f'
#define MOTOR_BACKWARD 'b'
#define MOTOR_LEFT 'l'
#define MOTOR_RIGHT 'r'
#define MOTOR_STOP 's'

#define SERVO_UP 'u'
#define SERVO_DOWN 'd'
#define SERVO_IN 'i'
#define SERVO_OUT 'o'

// Sensors
const int sensorEchoPin = -1;
const int sensorTrigPin = -1;

// Motors
const int rightMotorLogicPin1 = -1;
const int rightMotorLogicPin2 = -1;
const int leftMotorLogicPin1 = -1;
const int leftMotorLogicPin2 = -1;

int speed = 0;
char direction = MOTOR_STOP;
long duration;
int distance;

// Servos
Servo servo1;
const int servoPin = -1;
int angle = 130;
int deltaAngle = 3;

// Setup
void setup() { 
  Serial.begin(9600);
  Serial.println("Program: Motor Controller"); 
  Serial.println("Initializing ...");
  
  initializePins();
  
  servo1.attach(servoPin);
  servo1.write(angle);
  Serial.println("Initialization complete"); 
}

void loop() {
  char command = receiveCommand();

  if (command) {
    if (command == MOTOR_FORWARD || command == MOTOR_BACKWARD || command == MOTOR_LEFT || command == MOTOR_RIGHT || command == MOTOR_STOP) {
      updateMotorDirection(command);
      updateMotorSpeed(command);
    } else if (command == SERVO_UP || command == SERVO_DOWN || command == SERVO_IN || command == SERVO_OUT) {
      updateServo(command);
    } else if (command == EXIT) {
      Serial.println("Exiting program...");
      Serial.println("Goodbye");
      delay(100);
      while (true);  // Infinite loop to effectively "stop" the program
    } else {
      setMotorPins();
    }
  }

  checkSensorDistance();  // Check distance regularly
  delay(1000);
}

// Subroutine

void initializePins() {
  pinMode(rightMotorLogicPin1, OUTPUT);
  pinMode(rightMotorLogicPin2, OUTPUT);
  pinMode(leftMotorLogicPin1, OUTPUT);
  pinMode(leftMotorLogicPin2, OUTPUT);
  pinMode(sensorTrigPin, OUTPUT);
  pinMode(sensorEchoPin, INPUT);

  Serial.println("Pins initialized");
}

// Functions
char receiveCommand() {
  if (Serial.available() > 0) {
    return Serial.read();
  }
  return 0;
}

void updateMotorDirection(char command) {
  if (command == MOTOR_FORWARD || command == MOTOR_BACKWARD || command == MOTOR_LEFT || command == MOTOR_RIGHT) {
    direction = command;
    Serial.print("Direction set to: ");
    Serial.println(direction);
  } else if (command == MOTOR_STOP) {
    direction = MOTOR_STOP;
    Serial.println("Stopped");
  }
}

void updateMotorSpeed(char command) {
  if (command >= '1' && command <= '5') { 
    speed = (command - '0') * 10;
    Serial.print("Speed set to: ");
    Serial.print(speed);
    Serial.println("%");
  }
}

void setMotorPins() {
  int pwmDutyCycle = (speed * 255) / 50; // Scale speed to 0-255 PWM

  if (direction == MOTOR_FORWARD) {
    analogWrite(rightMotorLogicPin1, pwmDutyCycle);
    analogWrite(rightMotorLogicPin2, 0);
    analogWrite(leftMotorLogicPin1, pwmDutyCycle);
    analogWrite(leftMotorLogicPin2, 0);
  } else if (direction == MOTOR_BACKWARD) {
    analogWrite(rightMotorLogicPin1, 0);
    analogWrite(rightMotorLogicPin2, pwmDutyCycle);
    analogWrite(leftMotorLogicPin1, 0);
    analogWrite(leftMotorLogicPin2, pwmDutyCycle);
  } else if (direction == MOTOR_LEFT) {
    analogWrite(rightMotorLogicPin1, pwmDutyCycle);
    analogWrite(rightMotorLogicPin2, 0);
    analogWrite(leftMotorLogicPin1, 0);
    analogWrite(leftMotorLogicPin2, pwmDutyCycle);
  } else if (direction == MOTOR_RIGHT) {
    analogWrite(rightMotorLogicPin1, 0);
    analogWrite(rightMotorLogicPin2, pwmDutyCycle);
    analogWrite(leftMotorLogicPin1, pwmDutyCycle);
    analogWrite(leftMotorLogicPin2, 0);
  } else if (direction == MOTOR_STOP) {
    analogWrite(rightMotorLogicPin1, 0);
    analogWrite(rightMotorLogicPin2, 0);
    analogWrite(leftMotorLogicPin1, 0);
    analogWrite(leftMotorLogicPin2, 0);
  }
  Serial.println("Motors updated");
}

void checkSensorDistance() {
  digitalWrite(sensorTrigPin, LOW);
  delayMicroseconds(2);
 
  digitalWrite(sensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrigPin, LOW);

  duration = pulseIn(sensorEchoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);
}

void updateServo(char command) {
  if (command == SERVO_UP && angle + deltaAngle <= 180) {
    angle += deltaAngle;
  } else if (command == SERVO_DOWN && angle - deltaAngle >= 0) {
    angle -= deltaAngle;
  } else if (command == SERVO_IN) {
    angle = 45;
  } else if (command == SERVO_OUT) {
    angle = 130;
  }
  servo1.write(angle);
  Serial.print("Angle: ");
  Serial.println(angle);
}