#include <AFMotor.h>
// Direction values

// Motor control
#define FRONT_LEFT   4 // M4 on the driver shield front left
#define BACK_LEFT 1 // M1 on the driver shield done back left
#define FRONT_RIGHT   3 // M3 on the driver shield front right
#define BACK_RIGHT  2 // M2 on the driver shield back right

AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

void move(float speed, int direction)
{
  int speed_scaled = (speed / 100.0) * 255;
  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);

  switch(direction)
    {
      case BACK: 
        motorFL.run(BACKWARD);
        motorFR.run(BACKWARD);
        motorBL.run(BACKWARD);
        motorBR.run(BACKWARD);
        break;
      
      case GO: 
        motorFL.run(FORWARD);
        motorFR.run(FORWARD);
        motorBL.run(FORWARD);
        motorBR.run(FORWARD); 
        break;
      
      case CW: 
        motorFL.run(FORWARD);
        motorFR.run(BACKWARD);
        motorBL.run(FORWARD);
        motorBR.run(BACKWARD); 
        break;
      
      case CCW:
        motorFL.run(BACKWARD);
        motorFR.run(FORWARD);
        motorBL.run(BACKWARD);
        motorBR.run(FORWARD); 
        break;
      case STOP:
      default:
        motorFL.run(BRAKE);
        motorFR.run(BRAKE);
        motorBL.run(BRAKE);
        motorBR.run(BRAKE); 
    }
}

void forward(float dist, float speed)
{
  if (dist > 0)
    deltaDist = dist;
  else
    deltaDist = 9999999;
    
  newDist = forwardDist + deltaDist;
  
  dir = (TDirection) FORWARD;  
  move(speed, GO); 
}

void backward(float dist, float speed)
{
  if (dist > 0)
    deltaDist = dist;
  else
    deltaDist = 9999999;

  newDist = reverseDist + deltaDist;
  
  dir = (TDirection) BACKWARD;  
  move(speed, BACK);
}

void ccw(float angle, float speed)
{
  dir = (TDirection) LEFT;
  move(speed, LEFT);
}

void cw(float angle, float speed)
{
  dir = (TDirection) RIGHT;
  move(speed, RIGHT);
}

void stop()
{
  switch (dir) {
  case LEFT:
  cw(0, 100);
  break;
  case RIGHT:
  ccw(0, 100);
  break;
  case FORWARD:
  backward(0, 100);
  break;
  case BACKWARD:
  forward(0, 100);
  break;
  }
  delay(25);
  dir = (TDirection) STOP;
  move(0, STOP);
}
