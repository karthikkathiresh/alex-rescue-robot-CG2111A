#include <Servo.h>

#define SERVO_LEFT 40
#define SERVO_RIGHT 42
#define SERVO_BACK 33
#define ANGLE 45


Servo l_servo;  // create servo object to control a servo
Servo r_servo;
Servo b_servo;


void servoSetup() {
  l_servo.attach(SERVO_LEFT);  // attaches the servo on pin 9 to the servo object
  r_servo.attach(SERVO_RIGHT);
  b_servo.attach(SERVO_BACK);
  l_servo.write(180);
  r_servo.write(45);
  b_servo.write(60);
  
}

void open() {
  l_servo.write(180);
  r_servo.write(45);
}

void close() {
  l_servo.write(135);
  r_servo.write(90);
}

void open_back() {
    b_servo.write(180);
   delay(1000);
   b_servo.write(60);
}
