#include "imu.h"

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
int16_t accelGyro[6]={0}; 
int rslt;

bool inAir = false;
bool inAirRaw = false;
int timesNotInAir = 0;
int timesInAir = 0;
bool inAirSignal = false;
bool toggledInAirSignal = false;

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

void UpdateIMU() {
    int i = 0;
  ReadIMU();
  //get both accel and gyro data from bmi160
  //parameter accelGyro is the pointer to store the data
  if(rslt == 0){
    // magnitude
    double mag = sqrt(accelGyro[3]*accelGyro[3] + accelGyro[4]*accelGyro[4] + accelGyro[5]*accelGyro[5])/16384.0;
    
    inAirRaw = mag < 0.7;
    if(inAirRaw){
      timesNotInAir = 0;
      timesInAir++;
    } else {
      timesNotInAir++;
      timesInAir = 0;
    }

    inAirSignal = false;

    if(timesInAir >= 2) {
      inAir = true;
      inAirSignal = !toggledInAirSignal;
      toggledInAirSignal = true;
    } else if(timesNotInAir >= 2) {
      inAir = false;
      toggledInAirSignal = false;
    }

  }else{
    Serial.println("err");
  }
  
}