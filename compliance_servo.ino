/*
  This program has been created by Troy Duncan (z5592602) to create and test a component of 
  a DESN1000 component.

  When a serial connection is able to be made the arduino will connect with a laptop.
  
  To begin a command type 'B' then an angle between 0 and 180 degrees, the program will read the next three integers and
  if the position is valid will move.
*/

// Compliance Testing
// - changes servo angle

#include <Servo.h>
Servo servo1;
int servoPin = 9;

int currentPosition = 0;
char receivedChar;
boolean newData = false;
int angle = 130;
int deltaAngle = 3;

void setup() {
  Serial.begin(9600);
  servo1.attach(servoPin);
  servo1.write(angle);
  Serial.println("<Arduino is ready>");
}

void loop() {
  if (Serial.available() > 0) {
    receivedChar = Serial.read();
    newData = true;
  }
  if (newData == true && receivedChar == 'U') {
    if (angle + deltaAngle <= 180) {
      angle = angle + deltaAngle;
    }
    servo1.write(angle);
    Serial.print("Angle: ");
    Serial.println(angle);
    newData = false;
  } 
  if (newData == true && receivedChar == 'D') {
    if (angle - deltaAngle >= 0) {
      angle = angle - deltaAngle;
    }
    servo1.write(angle);
    Serial.print("Angle: ");
    Serial.println(angle);
    newData = false;
  }
  if (newData == true && receivedChar == 'I') {
    angle = 45;
    servo1.write(angle);
    Serial.print("Angle: ");
    Serial.println(angle);
    newData = false;
  }
  if (newData == true && receivedChar == 'O') {
    angle = 130;
    servo1.write(angle);
    Serial.print("Angle: ");
    Serial.println(angle);
    newData = false;
  }
}