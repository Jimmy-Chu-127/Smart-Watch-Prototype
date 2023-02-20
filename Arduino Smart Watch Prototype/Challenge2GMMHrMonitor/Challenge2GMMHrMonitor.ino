// Global variables
int ax = 0; int ay = 0; int az = 0;      // Acceleration values recorded from the readAccelSensor() function
int ppg = 0;                             // PPG from readPhotoSensor() (in Photodetector tab)
int sampleTime = 0;                      // Time of last sample (in Sampling tab)
bool sending;

// put your setup code here, to run once:
// Initialize the various components of the wearable
void setup() {
  setupAccelSensor();
  setupCommunication();
  setupDisplay();
  setupPhotoSensor();
  sending = false;
  writeDisplay("Sleep", 0, true);
  Serial.begin(9600);
}

// put your main code here, to run repeatedly:
// The main processing loop
void loop() {
  String command = receiveMessage();

  if(command == "sleep") {
    sending = false;
    writeDisplay("Sleep", 0, true);
  }
  else if(command == "wearable") {
    sending = true;
    writeDisplay("Wearable", 0, true);
  }
  else{
    if(command == "-1"){
      writeDisplay("Heartrate:", 0, false);
      writeDisplay(command.c_str(), 1, true);
    }
    else{
      writeDisplay("Heartrate:", 0, false);
      writeDisplay(command.c_str(), 1, false);
    }  
  }

  if(sending && sampleSensors()) {
    String response = String(sampleTime) + ",";
    //response += String(ax) + "," + String(ay) + "," + String(az);
    //response += "," + String(ppg);
    response += String(ppg);
    sendMessage(response);
  }
}