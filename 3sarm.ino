#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

const int pin1 = 9;
const int pin2 = 10;
const int pin3 = 11;

void setup() {
  servo1.attach(pin1);
  servo2.attach(pin2);
  servo3.attach(pin3);
}

void loop() {
  for (int angle = 0; angle <= 100; angle += 1) {
    servo1.write(angle);
    servo2.write(angle);
    servo3.write(angle);
    delay(15);
  }
  
  delay(1000);

  for (int angle = 100; angle >= 0; angle -= 1) {
    servo1.write(angle);
    servo2.write(angle);
    servo3.write(angle);
    delay(15);
  }
  
  delay(1000);
}
