#include "imu.h"
#include "main.h"
#include <vector>

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
int16_t accelGyro[6]={0}; 
int rslt;

bool inAir = false; // smoothed value wether IMU thinks it's in the air
bool inAirRaw = false; // raw value wether IMU thinks it's in the air
int timesNotInAir = 0; // times the IMU thinks it's not in the air consecutively
long timeOfFirstNotInAirMeasurement = 0; // time of the first not in air measurement in ms
int timesInAir = 0; // times the IMU thinks it's in the air consecutively
long timeOfFirstInAirMeasurement = 0; // time of the first in air measurement in ms
bool inAirStartSignal = false; // signal that we've just started being in the air this update
long inAirStartSignalTime = 0; // time we've just started being in the in ms

bool inAirMiddleSignal = false; // signal that we're in the middle of being in the air (approximated based on last signals)


bool inAirEndSignal = false; // signal that we've just stopped being in the air this update
long inAirEndSignalTime = 0; // time we've just stopped being in the in ms
long inAirSignalTime = 0; // time we've been in the air in ms
bool toggledInAirStartSignal = false; // whether or not we already toggled the start signal
bool toggledInAirMiddleSignal = false; // whether or not we already toggled the middle signal
bool toggledInAirEndSignal = false; // whether or not we already toggled the end signal

bool toggledBeatSignal = false; // whether or not we already toggled the jump signal
bool beatSignal = false; // signal that we've just jumped this update

long currentJumpLength = 0; // Length from start of last jump to start of jump in ms
std::vector<long> jumpLengths = {0};
long avgJumpLength = 0;
const int maxJumpLengths = 3;
double bpm = 0;
double bps = 0;
bool isStrongBeat = false;

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
long correctedTime = 0;

void UpdateVariables() {
  // Record whether or not we're in the air according to the measurement
  inAirRaw = accelerationMagnitude < 0.7;
  correctedTime = millis();

  // Count the times the IMU thinks it's in the air.
  if(inAirRaw){
    if(timesInAir == 0) {
      timeOfFirstInAirMeasurement = correctedTime; // set time of first measurement for later use
    }
    timesInAir++;
    timesNotInAir = 0;
  } else {
    if(timesNotInAir == 1) {
      timeOfFirstNotInAirMeasurement = correctedTime; // set time of first measurement for later use
    }
    timesNotInAir++;
    timesInAir = 0;
  }

  // Assume we haven't just started being in air
  inAirStartSignal = false;
  inAirEndSignal = false;

  if(timesInAir >= 2) {
    inAir = true;
    if(!toggledInAirStartSignal) {
      currentJumpLength = correctedTime - inAirStartSignalTime;
      jumpLengths.push_back(currentJumpLength);
      inAirStartSignalTime = timeOfFirstInAirMeasurement;
      inAirStartSignal = true;
      toggledBeatSignal = false;
    }
    toggledInAirStartSignal = true;

    toggledInAirEndSignal = false;
  } else if(timesNotInAir >= 2) {
    inAir = false;


    if(!toggledInAirEndSignal) {
      inAirEndSignalTime = timeOfFirstNotInAirMeasurement;
      inAirEndSignal = true;

      inAirSignalTime = inAirEndSignalTime - inAirStartSignalTime;
      
    }
    toggledInAirEndSignal = true;
    toggledInAirStartSignal = false;
  }

  // Trigger estimated air signals
  inAirMiddleSignal = false;
  if(inAir) {
    if(inAirStartSignalTime + inAirSignalTime/2 <= correctedTime && !toggledInAirMiddleSignal) {
      inAirMiddleSignal = true;
      toggledInAirMiddleSignal = true;
    }
  } else {
    toggledInAirMiddleSignal = false;
  }

  // Calculate average jump length
  if(jumpLengths.size() > maxJumpLengths) {
    jumpLengths.erase(jumpLengths.begin());
  }
  avgJumpLength = 0;
  for(int i=0; i<jumpLengths.size(); i++) {
    avgJumpLength += jumpLengths[i];
  }
  avgJumpLength /= jumpLengths.size();
  
  // calculate bpm based on avg jump length
  bps = 1000.0 / static_cast<double>(avgJumpLength);
  bpm = bps * 60.0;

  // Trigger beat signal
  beatSignal = false;
  if(inAirStartSignalTime + inAirSignalTime / 2 + avgJumpLength / 2 <= correctedTime && !toggledBeatSignal) {
    toggledBeatSignal = true;
    isStrongBeat ^= true;
    beatSignal = true;
    Serial.print("beat ");
    Serial.println(correctedTime);
    Serial.print("bpm ");
    Serial.println(bpm);
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