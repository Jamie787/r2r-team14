// Compliance Testing
// - prints distance detected by sensor

int sensorTrigPin = 7;
int sensorEchoPin = 8;

long duration;
int distance;

void setup()
{
  Serial.begin(9600);
  Serial.println("Program: Sensor Test"); 
  Serial.println("Initializing ...");

  pinMode(sensorTrigPin, OUTPUT);
  pinMode(sensorEchoPin, INPUT);
  
  Serial.println("Initialization complete"); 
}

void loop()
{
  checkSensorDistance();  // Check distance regularly
  delay(1000);
}

void checkSensorDistance() {
  // Clear sensorTrigPin
  digitalWrite(sensorTrigPin, LOW);
  delayMicroseconds(2);
 
  // Set sensorTrigPin on HIGH
  digitalWrite(sensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrigPin, LOW);

  // Reads sensorEchoPin
  duration = pulseIn(sensorEchoPin, HIGH);
  
  // Calculate distance
  distance = duration * 0.034 / 2;
  
  // Print distance
  Serial.print("Distance: ");
  Serial.println(distance);
}