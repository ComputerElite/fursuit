#include "imu.h"

DFRobot_BMI160 bmi160;

void InitIMU() {
    //init the hardware bmin160  
    if (bmi160.softReset() != BMI160_OK){
        Serial.println("reset false");
        while(1);
    }
    
    //set and init the bmi160 i2c address
    if (bmi160.I2cInit(i2c_addr) != BMI160_OK){
        Serial.println("init false");
        while(1);
    }
}

void UpdateIMU() {
    
}