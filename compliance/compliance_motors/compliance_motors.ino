// =======================================================
// DESN1000 - Computing Technical Stream
// Motor Controller
// Written by Jamie Gold
// Controls motors via an H Bridge 
// ======================================================= 

// -------------------------------------------------------
// Global Variables
// -------------------------------------------------------
const int rightMotorLogicPin1 = 5;    
const int rightMotorLogicPin2 = 6;  
const int leftMotorLogicPin1 = 10;    
const int leftMotorLogicPin2 = 11; 

const int trigPin = 2;
const int echoPin = 4;

int pwmDutyCycle = 0;

char currentDirection = 'f';

// -------------------------------------------------------
// The setup() method runs once, when the sketch starts
// -------------------------------------------------------
void setup(){ 
 
  // Initialize the serial communications
  Serial.begin(9600);

  // Print the program details
  Serial.println("-------------------------------------");
  Serial.println("Program: motors are awesome ! ! !"); 
  Serial.println("Initializing Motors");

  initialiseMotors();
  initialiseUltrasonic();

  // Initialization completed successfully
  Serial.println("Initialization complete");
}

// -------------------------------------------------------
// The loop() method runs over and over again
// -------------------------------------------------------
void loop(){
  
  // Check if we are at a safe operating distance
  checkSafetyDistance();

  char incomingByte = 0;

  // See if there's incoming serial data
  if (Serial.available()) {
    // Read the oldest byte in the serial buffer
    incomingByte = Serial.read();

    // If character is a direction
    changeDirection(incomingByte);
    
    // If character is a speed
    changeSpeed(incomingByte);

    // If the character was to exit
    tryExit(incomingByte);
  }

  updateMotors();
  delay(10);
}

void initialiseMotors() {
  // Configuration the motor pins

  pinMode(rightMotorLogicPin1, OUTPUT);
  pinMode(rightMotorLogicPin2, OUTPUT);
  pinMode(leftMotorLogicPin1, OUTPUT);
  pinMode(leftMotorLogicPin2, OUTPUT);

  // Print the motor pin configuration for wiring

  Serial.print("Right motor pin 1 = ");
  Serial.println(rightMotorLogicPin1);
  Serial.print("Right motor pin 2 = ");
  Serial.println(rightMotorLogicPin2);
  Serial.print("Left motor pin 1 = ");
  Serial.println(leftMotorLogicPin1);
  Serial.print("Left motor pin 2 = ");
  Serial.println(leftMotorLogicPin2);
} 

void changeDirection(char incomingByte) {
  // Is this a direction; 'f' 'b' 'l' 'r'
  if (incomingByte == 'f') {
    Serial.println("Forwards");
  }
  else if (incomingByte == 'b') {
    Serial.println("Backward");
  }
  else if (incomingByte == 'l') {
    Serial.println("Left");
  }
  else if (incomingByte == 'r') {
    Serial.println("Right");
  }
  else {
    return;
  }

  currentDirection = incomingByte;
}

void changeSpeed(char incomingByte) {
  // Is this a motor speed '0' - '5'
  if (incomingByte == '0') {
    pwmDutyCycle = 0;
    Serial.println("Stop");
  }

  if (incomingByte >= '1' && incomingByte <= '5') {
    float speed = incomingByte - 48;
    pwmDutyCycle = 255 * speed / 10;

    Serial.print("Motors at ");
    Serial.print(incomingByte);
    Serial.println("0% speed");
  }
}

void updateMotors() {
  if (currentDirection == 'f') {
    analogWrite(leftMotorLogicPin1, 0);
    analogWrite(leftMotorLogicPin2, pwmDutyCycle);
    analogWrite(rightMotorLogicPin1, 0);
    analogWrite(rightMotorLogicPin2, pwmDutyCycle);
  }
  else if (currentDirection == 'b') {
    analogWrite(leftMotorLogicPin1, pwmDutyCycle);
    analogWrite(leftMotorLogicPin2, 0);
    analogWrite(rightMotorLogicPin1, pwmDutyCycle);
    analogWrite(rightMotorLogicPin2, 0);
  }
  else if (currentDirection == 'l') {
    analogWrite(leftMotorLogicPin1, pwmDutyCycle);
    analogWrite(leftMotorLogicPin2, 0);
    analogWrite(rightMotorLogicPin1, 0);
    analogWrite(rightMotorLogicPin2, pwmDutyCycle);
  }
  else if (currentDirection == 'r') {
    analogWrite(leftMotorLogicPin1, 0);
    analogWrite(leftMotorLogicPin2, pwmDutyCycle);
    analogWrite(rightMotorLogicPin1, pwmDutyCycle);
    analogWrite(rightMotorLogicPin2, 0);
  }
}

void tryExit(char incomingByte) {
  if (incomingByte == 'x') {
    Serial.println("Exiting");
    while (true);
  }
}

void initialiseUltrasonic() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print("trigPin = ");
  Serial.println(trigPin);
  Serial.print("echoPin = ");
  Serial.println(echoPin);
}

float findDistance() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration / 2 * 0.034;

  return distance;
}

void checkSafetyDistance() {
  static int distanceTimer = 250;
  float distance = findDistance();

  if (distance < 30 && distanceTimer >= 250) {
    Serial.print("Distance is currently ");
    Serial.print(distance);
    Serial.println(" cm away");

    distanceTimer = 0;
  }

  distanceTimer++;
}