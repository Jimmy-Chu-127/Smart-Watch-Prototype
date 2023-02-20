// setting PWM properties 
const int pwmFrequency = 5000;               // Set the PWM frequency to 5KHz
const int pwmChannel = 0;                    // Use PWM channel 0
const int pwmBitResolution = 8;              // Set a PWM resolution of 8 bits

const int MOTOR_PIN = 18;                    // Set the GPIO pin for motor

//The setupMotor() function configures a PWM channel and then
//pin an output pin MOTOR_PIN 18 to the PWM channel
void setupMotor(){
  // configure PWM on a specific pwmChannel
  ledcSetup(pwmChannel, pwmFrequency, pwmBitResolution);
  // attach the pwmChannel to the output GPIO to be controlled
  ledcAttachPin(MOTOR_PIN, pwmChannel);
}

//The activateMotor() function changes the duty cycle of the
//pwm channel according to motorPower for MOTOR_PIN
void activateMotor(int motorPower){
  ledcWrite(pwmChannel, motorPower);         //changing the duty cycle of the pwm
}

void deactivateMotor(){
  ledcWrite(pwmChannel, 0);                  //changing the duty cycle of the pwm to 0
}