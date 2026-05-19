#include <Servo.h>

// Create a servo object to control a servo motor
Servo myServo;  

// Define the signal pin connected to the servo
const int servoPin = 9; 

void setup() {
  // Attaches the servo on pin 9 to the servo object
  myServo.attach(servoPin); 
}

void loop() {
  // 1. Move from 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle += 1) {
    myServo.write(angle);              // Tell servo to go to position in variable 'angle'
    delay(15);                         // Wait 15ms for the servo to reach the position
  }
  
  delay(1000);                         // Wait 1 second at 180 degrees

  // 2. Move back from 180 to 0 degrees
  for (int angle = 180; angle >= 0; angle -= 1) {
    myServo.write(angle);              // Tell servo to go to position in variable 'angle'
    delay(15);                         // Wait 15ms for the servo to reach the position
  }
  
  delay(1000);                         // Wait 1 second at 0 degrees
}
