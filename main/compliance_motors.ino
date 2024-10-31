// Compliance Testing
// - turns motor on

const int motorLogicPin1 = 5;  // Right motor pin 1
const int motorLogicPin2 = 6;  // Right motor pin 2
const int motorLogicPin3 = 10;  // Left motor pin 1
const int motorLogicPin4 = 11; // Left motor pin 2

int speed = 0;
char direction = 's'; // 's'= stop

void setup() {
  Serial.begin(9600);
  Serial.println("Program: Robot Motor Controller");
  Serial.println("Initalising...");

  // set all motor pins as output and print message
  pinMode(motorLogicPin1, OUTPUT);
  Serial.println("Right Motor Pin 1 = 10");
  pinMode(motorLogicPin2, OUTPUT);
  Serial.println("Right Motor Pin 2 = 11");
  pinMode(motorLogicPin3, OUTPUT);
  Serial.println("Left Motor Pin 1 = 3");
  pinMode(motorLogicPin4, OUTPUT);
  Serial.println("Left Motor Pin 2 = 9");
  
  Serial.println("Commands: forward, backward, left, right, stop");
  Serial.println("Speed: 0-100");
}

// Helper function to update the motor pins
void updateMotors() {
  int pwmDutyCycle = (speed * 255) / 50; // scale speed to 0-255 PWM value
  
  if (direction == 'f') {
    // move forward
    analogWrite(motorLogicPin1, pwmDutyCycle);
    analogWrite(motorLogicPin2, 0);
    analogWrite(motorLogicPin3, pwmDutyCycle);
    analogWrite(motorLogicPin4, 0);
    
  } else if (direction == 'b') {
    // move backwards
    analogWrite(motorLogicPin1, 0);
    analogWrite(motorLogicPin2, pwmDutyCycle);
    analogWrite(motorLogicPin3, 0);
    analogWrite(motorLogicPin4, pwmDutyCycle);
    
  } else if (direction == 'l') {
    // turn left
    analogWrite(motorLogicPin1, pwmDutyCycle);
    analogWrite(motorLogicPin2, 0);
    analogWrite(motorLogicPin3, 0);
    analogWrite(motorLogicPin4, pwmDutyCycle);
    
  } else if (direction == 'r') {
    // turn right
    analogWrite(motorLogicPin1, 0);
    analogWrite(motorLogicPin2, pwmDutyCycle);
    analogWrite(motorLogicPin3, pwmDutyCycle);
    analogWrite(motorLogicPin4, 0);
    
  } else if (direction == 's') {
    // stop
    analogWrite(motorLogicPin1, 0);
    analogWrite(motorLogicPin2, 0);
    analogWrite(motorLogicPin3, 0);
    analogWrite(motorLogicPin4, 0);
  }
  
  Serial.println("Motors updated");
}

void loop(){
 if (Serial.available() > 0) {
   char input = Serial.read(); // read input
   
   if (input == 'f' || input == 'b' || input == 'l' || input == 'r') {
     direction = input;
     Serial.print("Direction set to: ");
     Serial.println(direction);
     updateMotors();
   } else if (input == '0 ') {
     direction = 's'; 
     Serial.println("Stopped");
     updateMotors();
   } else if (input >= '1 ' && input <= '5 ') { 
     speed = (input - '0') * 10;
     Serial.print("Speed set to: ");
     Serial.print(speed);
     Serial.println("%");
     updateMotors();
   } else {
     Serial.println("Invalid command");
   }
 }
}