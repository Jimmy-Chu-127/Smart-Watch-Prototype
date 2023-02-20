const int LED_PIN = 14;                 //map LED_PIN to the GPIO pin 14
const int BUTTON_PIN1 = 15;             //map BUTTON_PIN1 to the GPIO pin 15
const int BUTTON_PIN2 = 16;             //map BUTTON_PIN2 to the GPIO pin 16

//state == 0  represents the Pedometer state
//state == 10 represents the Heartrate Monitor state
//state == 20 represents the Weather Watch state
//state == 30 represents the Idle Detecter state
//state == 31 represents the Idle Detecter OFF state
//state == 32 represents the Idle Detecter LED Blinking state
//state == 33 represents the Idle Detecter Motor Buzzing state
int state = 0;                          //initializing state

int button1_now_state;                  //current state of button1
int button1_pre_state = HIGH;           //previous state of button1
int button2_now_state;                  //current state of button2
int button2_pre_state = HIGH;           //previous state of button2

int LED = LOW;                          //initializing a variable LED
unsigned long now_time;
unsigned long LED_time;
unsigned long buz_time;

int sampleTime = 0;                     // Time of last sample (in Sampling tab)
int ax = 0; int ay = 0; int az = 0;     // Acceleration (from readAccelSensor())
int ppg = 0;                            // PPG from readPhotoSensor() (in Photodetector tab)
bool sending;

String msg1 = "ON"; 
String msg2 = "OFF";
String msg3 = "Idle 5s";
String msg4 = "Idle 10s";
String msg5 = "Great Job";
String msg6 = "s";                      //msg6 for Python to switch
String msg7 = "";                       //msg7 for clearing

String command1 = "";                   //step count
String command2 = "";                   //Heartrate
String command3 = "";                   //time
String command4 = "";                   //date
String command5 = "";                   //weather
String command6 = "";                   //location
String command7 = "";                   //command
String command8 = "";                   //combination

//blink the LED
void LED_blink(){
  if(now_time - LED_time >= 500){
    LED_time = millis();

    if(LED == LOW){
      LED = HIGH;
    }
    else{
      LED = LOW;
    }
    digitalWrite(LED_PIN, LED);          //toggle the LED_PIN output
  }
}

//turning off the LED
void LED_off(){      
  digitalWrite(LED_PIN, LOW);           
}

//The press_checker() function detects button state transition
//returns 1 for a button press
//returns 0 for no button press
int press_checker1() {
  if (button1_pre_state == button1_now_state) {         //no transition occurs
    if (button1_now_state == HIGH){                     
    }
    else { //button1_now_state == LOW                   //if the button stays LOW there is a possible chance to reach 3s worth resetting
    }
    button1_pre_state = button1_now_state;              //updating the button state
    return 0;
  }
  else {  //button_pre_state != button_now_state        //transition occurs
    if (button1_now_state == HIGH){                     //transition from LOW to HIGH consider pressed
      button1_pre_state = button1_now_state;            //updating the button state
      return 1;
    }
    else { //button1_now_state == LOW                   //transition from HIGH to LOW not consider pressed
      if(button1_pre_state == HIGH){
      }
      button1_pre_state = button1_now_state;            //updating the button state
      return 0;
    }
  }
}

int press_checker2() {
  if (button2_pre_state == button2_now_state) {         //no transition occurs
    if (button2_now_state == HIGH){                     
    }
    else { //button2_now_state == LOW                   //if the button stays LOW there is a possible chance to reach 3s worth resetting
    }
    button2_pre_state = button2_now_state;              //updating the button state
    return 0;
  }
  else {  //button_pre_state != button_now_state        //transition occurs
    if (button2_now_state == HIGH){                     //transition from LOW to HIGH consider pressed
      button2_pre_state = button2_now_state;            //updating the button state
      return 1;
    }
    else { //button_now_state == LOW                    //transition from HIGH to LOW not consider pressed
      if(button2_pre_state == HIGH){
      }
      button2_pre_state = button2_now_state;            //updating the button state
      return 0;
    }
  }
}

// put your setup code here, to run once:
void setup() {
  setupAccelSensor();                   //setting up accelerometer
  setupPhotoSensor();                   //setting up photoresistor
  setupCommunication();                 //setting up communication
  setupDisplay();                       //setting up display
  setupMotor();                         //setting up buzzer
  pinMode(LED_PIN, OUTPUT);             //pin the LED
  pinMode(BUTTON_PIN1, INPUT_PULLUP);   //pin Button 1
  pinMode(BUTTON_PIN2, INPUT_PULLUP);   //pin Button 2
  sending = false;                      //initializing sending
  writeDisplay("Sleep", 0, true);       //initializing display
}

// put your main code here, to run repeatedly:
void loop() {
  now_time = millis();                                  //getting the current time
  button1_now_state = digitalRead(BUTTON_PIN1);         //getting the current button1 state
  button2_now_state = digitalRead(BUTTON_PIN2);         //getting the current button2 state


  String command = receiveMessage();                    //receive message from python
  if(command != ""){
    int startIndex = 0;
    int index = command.indexOf(',', startIndex);
    command1 = command.substring(startIndex, index);
    startIndex = index + 1;

    index = command.indexOf(',', startIndex);
    command2 = command.substring(startIndex, index);
    startIndex = index + 1;

    index = command.indexOf(',', startIndex);
    command3 = command.substring(startIndex, index);
    startIndex = index + 1;

    index = command.indexOf(',', startIndex);
    command4 = command.substring(startIndex, index);
    startIndex = index + 1;

    index = command.indexOf(',', startIndex);
    command5 = command.substring(startIndex, index);
    startIndex = index + 1;

    index = command.indexOf(',', startIndex);
    command6 = command.substring(startIndex, index);
    startIndex = index + 1;

    index = command.indexOf(',', startIndex);
    command7 = command.substring(startIndex, index);
    
    command8 = command3 + "," + command4 + "," + command5 + "," + command6;
  }

  if(command != ""){
    if(command7 == "sleep") {                              //when command is sleep
      sending = false;
      writeDisplay("Sleep", 0, true);
    }
    else if(command7 == "wearable") {                      //when command is wearable
      sending = true;
      writeDisplay("Wearable", 0, true);
    }
  }

  if(sending && sampleSensors()) {                       //when sending is true and accelerometer is sampled
    String response = String(sampleTime) + ",";
    response += String(ax) + "," + String(ay) + "," + String(az) + "," + String(ppg);
    sendMessage(response);    
  }

  if(state == 0){                                        //Pedometer state
    if(command != ""){
      command1 = "Step count:" + command1;
      writeDisplay(command1.c_str(), 2, false);          //writing step count to display
    }

    if(press_checker1()){
      state = 10;
    }
  }

  if(state == 10){                                       //Heartrate Monitor state
    if(command != ""){
      if(command2 == "0"){
        writeDisplay("Heartrate:", 0, true);             //write Heartrate:
        writeDisplay(command2.c_str(), 1, false);        //write number and clear
      }
      else{
        writeDisplay("Heartrate:", 0, false);            //write Heartrate:
        writeDisplay(command2.c_str(), 1, false);        //write number but don't clear
      }
    }

    if(press_checker1()){
      state = 20;
    }
  }

  if(state == 20){                                      //Weather Watch State
    if(command != ""){
      writeDisplay(msg7.c_str(), 0, true);              //clearing the display
      writeDisplayCSV(command8, 3);                     //writing to the display
    }

    if(press_checker1()){
      state = 30;
    }
  }

  if(state == 31){                                      //Idle Detector OFF state
    writeDisplay(msg2.c_str(), 2, true);                //writing OFF to row 2
    if(press_checker2()){
      state = 30;
    }

    if(press_checker1()){
      state = 0;
    }
  }

  if(state == 30){                                      //Idle Detector ON state
    writeDisplay(msg1.c_str(), 2, true);                //writing ON to row 2
    writeDisplay(msg5.c_str(), 1, true);                //writing Great Job to row 1
    if(press_checker2()){
      state = 31;
    }
    if(command != ""){
      if(command7 == "idle"){
        state = 32;
        command7 = "stay";
      }
    }

    if(press_checker1()){
      state = 0;
    }
  }

  if(state == 32){                                       //Idle Detector LED Blinking state
    writeDisplay(msg1.c_str(), 2, true);                 //writing ON to row 2
    writeDisplay(msg3.c_str(), 1, true);                 //writing idle 5s to row 1
    LED_blink();
    if(press_checker2()){
      state = 31;
      LED_off();
    }

    if(command != ""){
      if(command7 == "idle"){
        state = 33;
        buz_time = millis();
        LED_off();
      }
      if(command7 == "motion"){
        state = 30;
        LED_off();
      }
    }

    if(press_checker1()){
      state = 0;
    }
  }

  if(state == 33){                                        //Idle Detector Motor Buzzing state
    writeDisplay(msg1.c_str(), 2, true);                  //writing ON to row 2
    writeDisplay(msg4.c_str(), 1, true);                  //writing Idle 10s to row 1
    if(now_time - buz_time < 1000){
      activateMotor(125);
    }
    else{ //now_time - buz_time >= 1000
      deactivateMotor();
    }

    if(press_checker2()){
      state = 31;
    }

    if(command != ""){
      if(command7 == "motion"){
        state = 30;
      }
    } 

    if(press_checker1()){
      state = 0;
    }
  }

}
