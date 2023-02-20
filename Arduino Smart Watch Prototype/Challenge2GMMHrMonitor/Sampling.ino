int sampleRate = 50;                           //Sensor reading Frequency (Hz)
unsigned long sampleDelay = 1e6/sampleRate;    //Time(μs) between samples
unsigned long timeStart = 0;                   //Start time timing variable
unsigned long timeEnd = 0;                     //End time timing variable


//The sampleSensors() function samples all the sensors at the defined sample
//rate. It will return true if new samples are read in, and false otherwise
bool sampleSensors(){
  timeEnd = micros();                          //getting the current time
  if(timeEnd - timeStart >= sampleDelay) {     //sampleDelay has passed
    displaySampleRate(timeEnd);
    timeStart = timeEnd;

    // Read the sensors and store their outputs in global variables
    sampleTime = millis();                     //getting the current time
    //readAccelSensor();                       //values stored in "ax", "ay", and "az"
    readPhotoSensor();                         //value stored in "ppg"
    return true;
  }

  return false;                                //sampleDelay has not yet passed
}


//The displaySampleRate() function calculates the effective sampling rate and
//display it on the OLED
void displaySampleRate(unsigned long currentTime) {
     int nSamples = 100;
     static int count = 0;
     static unsigned long lastTime = 0;

     count++;
     
     if(count == nSamples) {
      double avgRate = nSamples * 1e6 / (currentTime - lastTime);
      String message = String(avgRate) + " Hz";
      writeDisplay(message.c_str(), 3, false);

      // reset
      count = 0;
      lastTime = currentTime;
    }
}