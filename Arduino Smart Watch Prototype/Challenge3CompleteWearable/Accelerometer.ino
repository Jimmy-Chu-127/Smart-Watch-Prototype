const int X_PIN = A2;              //setting up X_PIN
const int Y_PIN = A3;              //setting up Y_PIN
const int Z_PIN = A4;              //setting up Z_PIN

//The function setupAccelSensor sets all the pins connected
//to the Accelerometer to input
void setupAccelSensor(){
  pinMode(X_PIN, INPUT);           //setting X_PIN as input
  pinMode(Y_PIN, INPUT);           //setting Y_PIN as input
  pinMode(Z_PIN, INPUT);           //setting Z_PIN as input
}

//The function readAccelSensor reads the input from X_PIN,
//Y_PIN, and Z_PIN, then store them into ax, ay, and az.
void readAccelSensor(){
  ax = analogRead(X_PIN);          //read from X_PIN and store the value to ax
  ay = analogRead(Y_PIN);          //read from Y_PIN and store the value to ay
  az = analogRead(Z_PIN);          //read from Z_PIN and store the value to az
}