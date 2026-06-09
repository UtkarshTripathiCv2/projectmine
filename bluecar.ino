#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// --- Right Motor Pins ---
const int enableRightMotor = 22; 
const int rightMotorPin1 = 16;
const int rightMotorPin2 = 17;

// --- Left Motor Pins ---
const int enableLeftMotor = 23;
const int leftMotorPin1 = 18;
const int leftMotorPin2 = 19;

// --- PWM Speed Settings ---
#define MAX_MOTOR_SPEED 255
const int PWMFreq = 1000;      // 1 KHz frequency
const int PWMResolution = 8;   // 8-bit resolution (0-255)

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  // --- Right Motor Direction ---
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);    
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);      
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);      
  }
  
  // --- Left Motor Direction ---
  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);    
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);      
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);      
  }
  
  // --- Apply Speed ---
  // UPDATED SYNTAX: We now write the speed directly to the enable pin!
  ledcWrite(enableRightMotor, abs(rightMotorSpeed));
  ledcWrite(enableLeftMotor, abs(leftMotorSpeed));  
}

void setUpPinModes() {
  // Set direction pins as standard outputs
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // --- Setup PWM for Speed ---
  // UPDATED SYNTAX: Attach frequency and resolution directly to the enable pins.
  ledcAttach(enableRightMotor, PWMFreq, PWMResolution);
  ledcAttach(enableLeftMotor, PWMFreq, PWMResolution); 

  // Ensure motors are stopped on startup
  rotateMotor(0, 0); 
}

void setup() {
  setUpPinModes();
  Dabble.begin("MyBluetoothCar"); 
}

void loop() {
  int rightMotorSpeed = 0;
  int leftMotorSpeed = 0;
  
  Dabble.processInput(); // Read Bluetooth data from phone
  
  // Determine speed and direction based on buttons
  if (GamePad.isUpPressed()) {
    rightMotorSpeed = MAX_MOTOR_SPEED;
    leftMotorSpeed = MAX_MOTOR_SPEED;
  }
  else if (GamePad.isDownPressed()) {
    rightMotorSpeed = -MAX_MOTOR_SPEED;
    leftMotorSpeed = -MAX_MOTOR_SPEED;
  }
  else if (GamePad.isLeftPressed()) {
    rightMotorSpeed = MAX_MOTOR_SPEED;
    leftMotorSpeed = -MAX_MOTOR_SPEED;
  }
  else if (GamePad.isRightPressed()) {
    rightMotorSpeed = -MAX_MOTOR_SPEED;
    leftMotorSpeed = MAX_MOTOR_SPEED;
  }

  // Send the final commands to the motors
  rotateMotor(rightMotorSpeed, leftMotorSpeed);
}
