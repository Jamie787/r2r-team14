// Compliance Testing
// - prints initalisation message

void setup()
{
  Serial.begin(9600);
  Serial.println("Program: Initalise Test"); 
  Serial.println("Initializing ...");
  
  Serial.println("Initialization complete"); 
}

void loop()
{
  Serial.println("Hello world");
  
  delay(1000);
}