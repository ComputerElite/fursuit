#include "imu.h"

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
int16_t accelGyro[6]={0}; 
int rslt;

bool inAir = false; // smoothed value wether IMU thinks it's in the air
bool inAirRaw = false; // raw value wether IMU thinks it's in the air
int timesNotInAir = 0; // times the IMU thinks it's not in the air consecutively
int timesInAir = 0; // times the IMU thinks it's in the air consecutively
bool inAirStartSignal = false; // signal that we've just started being in the air this update
long inAirStartSignalTime = 0; // time we've just started being in the in ms

bool inAirMiddleSignal = false; // signal that we're in the middle of being in the air (approximated based on last signals)


bool inAirEndSignal = false; // signal that we've just stopped being in the air this update
long inAirEndSignalTime = 0; // time we've just stopped being in the in ms
long inAirSignalTime = 0; // time we've been in the air in ms
bool toggledInAirStartSignal = false; // whether or not we already toggled the start signal
bool toggledInAirMiddleSignal = false; // whether or not we already toggled the middle signal
bool toggledInAirEndSignal = false; // whether or not we already toggled the end signal

double accelerationMagnitude = 0; // magnitude of the acceleration

void InitIMU() {
    if (bmi160.softReset() != BMI160_OK){
        Serial.println("reset false");
        while(1);
    }
    
    //set and init the bmi160 i2c address
    if (bmi160.I2cInit(i2c_addr) != BMI160_OK){
        Serial.println("init false");
        while(1);
    }
    //BMI160.begin(BMI160GenClass::I2C_MODE, i2c_addr);
}

bool ReadIMU() {
  rslt = bmi160.getAccelGyroData(accelGyro);
  if(rslt == 0){
    return true;
  }else{
    return false;
  }
}

void UpdateVariables() {
  // Record whether or not we're in the air according to the measurement
  inAirRaw = accelerationMagnitude < 0.7;

  // Count the times the IMU thinks it's in the air.
  if(inAirRaw){
    timesNotInAir = 0;
    timesInAir++;
  } else {
    timesNotInAir++;
    timesInAir = 0;
  }

  // Assume we haven't just started being in air
  inAirStartSignal = false;
  inAirEndSignal = false;

  if(timesInAir >= 2) {
    inAir = true;
    if(!toggledInAirStartSignal) {
      inAirStartSignalTime = millis();
      inAirStartSignal = true;
    }
    toggledInAirStartSignal = true;

    toggledInAirEndSignal = false;
  } else if(timesNotInAir >= 2) {
    inAir = false;


    if(!toggledInAirEndSignal) {
      inAirEndSignalTime = millis();
      inAirEndSignal = true;

      inAirSignalTime = inAirEndSignalTime - inAirStartSignalTime;
      
    }
    toggledInAirEndSignal = true;
    toggledInAirStartSignal = false;
  }

  inAirMiddleSignal = false;
  // Trigger estimated air signals
  if(inAir) {
    if(inAirStartSignalTime + inAirSignalTime/2 <= millis() && !toggledInAirMiddleSignal) {
      inAirMiddleSignal = true;
      toggledInAirMiddleSignal = true;
    }
  } else {
    toggledInAirMiddleSignal = false;
  }
}

void UpdateIMU() {
  int i = 0;
  ReadIMU();
  //get both accel and gyro data from bmi160
  //parameter accelGyro is the pointer to store the data
  if(rslt == 0){
    // magnitude
    accelerationMagnitude = sqrt(accelGyro[3]*accelGyro[3] + accelGyro[4]*accelGyro[4] + accelGyro[5]*accelGyro[5])/16384.0;
  }else{
    Serial.println("err");
  }
  UpdateVariables();
}