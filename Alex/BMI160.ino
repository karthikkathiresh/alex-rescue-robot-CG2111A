#include <DFRobot_BMI160.h>
#include <Wire.h>
#include <math.h>

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;

// Variables for orientation calculations
float gz;
float yaw;
unsigned long lastTime;
float dt = 0.01; // Time interval in seconds (10 ms)

void IMUsetup() {

  if (bmi160.softReset() != BMI160_OK) {
    dbprintf("failed to reset BMI160");
    while (1);
  }

  if (bmi160.I2cInit(i2c_addr) != BMI160_OK) {
    dbprintf("failed to init BMI160");
    while (1);
  }
  
}

void _rotate(float degree) { // hogs the arduino until rotation is reached
  while (abs(yaw/16.67) < degree) {
  unsigned long currentTime = millis();
  dt = (currentTime - lastTime) / 1000.0; // Update dt
  lastTime = currentTime;

  int16_t accelGyro[6] = {0};
  int rslt = bmi160.getAccelGyroData(accelGyro);

  if (rslt == 0) {
    gz = accelGyro[2] * 3.14 / 180.0;
    //Serial.println(gz);

    // Integrate gyroscope data to get yaw
    yaw += gz * dt * 180 / 3.14;

    // yaw
    //Serial.println(yaw);
    
  } else { 
    return;
  }
  

    if(Serial.available())
    return;
  
  delay(10);
  }
  return;
}

void rotate(float degree) {
  yaw = 0;
  lastTime = millis();
  delay(10);
  _rotate(degree);
  return;
}
