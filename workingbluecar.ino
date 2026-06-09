#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// --- Right Motor Pins ---
const int enableRightMotor = 25; 
const int rightMotorPin1 = 16;
const int rightMotorPin2 = 17;

// --- Left Motor Pins ---
const int enableLeftMotor = 26;
const int leftMotorPin1 = 18;
const int leftMotorPin2 = 19;

#define MAX_MOTOR_SPEED 100

// --- PWM Settings (Required for Core 2.x) ---
const int PWMFreq = 1000;
const int PWMResolution = 8;
const int rightMotorChannel = 4;
const int leftMotorChannel = 5;

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
  
  // --- Apply Speed to PWM Channels ---
  ledcWrite(rightMotorChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorChannel, abs(leftMotorSpeed));  
}

void setup() {
  // Set direction pins as standard outputs
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // --- Setup PWM Channels ---
  ledcSetup(rightMotorChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorChannel, PWMFreq, PWMResolution);
  
  // --- Attach enable pins to the channels ---
  ledcAttachPin(enableRightMotor, rightMotorChannel);
  ledcAttachPin(enableLeftMotor, leftMotorChannel);

  // Ensure motors are stopped on startup
  rotateMotor(0, 0); 
  
  // Start Bluetooth connection
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
