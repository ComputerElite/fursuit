#include "imu.h"
#include "main.h"
#include "server.h"
#include "LowPassFilter.hpp"
#include <vector>

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
int16_t accel[3]={0}; 
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

long magicJumpNumber = 150;// magic number for in air start offset an actual jump beginning


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
long nextBeatTime = 0;
long beatSignalTime = 0;

double accelerationMagnitudeRaw = 0; // magnitude of the acceleration
double accelerationMagnitude = 0; // magnitude of the acceleration (low pass filtered)


LowPassFilter lpf = LowPassFilter(4, 0.24);

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
char concatinated[100] ="";
char tmp[100];

void AppendDouble(double value) {
  std::string s = std::to_string(value);
  s.copy(tmp, s.length());
  tmp[s.length()] = '\0';
  strcat(concatinated, tmp);
}

void AppendLong(long value) {
  std::string s = std::to_string(value);
  s.copy(tmp, s.length());
  tmp[s.length()] = '\0';
  strcat(concatinated, tmp);
}
bool ReadIMU() {
  concatinated[0] = '\0';
  strcat(concatinated, "t");
  AppendLong(millis());
  rslt = bmi160.getAccelData(accel);
  strcat(concatinated, " x");
  AppendDouble(accel[0]/16384.0);
  strcat(concatinated, " y");
  AppendDouble(accel[1]/16384.0);
  strcat(concatinated, " z");
  AppendDouble(accel[2]/16384.0);
  if(serialOn) {
    Serial.print("x");
    Serial.println();
    Serial.print("y");
    Serial.println(accel[1]/16384.0);
    Serial.print("z");
    Serial.println(accel[2]/16384.0);
  }
  if(rslt == 0){
    return true;
  }else{
    return false;
  }
}
long correctedTime = 0;

void UpdateVariables() {
  // Record whether or not we're in the air according to the measurement
  inAirRaw = accelerationMagnitude > 0.2;
  correctedTime = millis();
  strcat(concatinated, " a");
  AppendDouble(accelerationMagnitude);
  strcat(concatinated, " r");
  AppendDouble(accelerationMagnitudeRaw);
  if(serialOn) {
    //Serial.print("t");
    //Serial.println(correctedTime);
    Serial.print("a");
    Serial.println(accelerationMagnitude);
  }

  // Count the times the IMU thinks it's in the air and thus smooth output
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

  bool lastBeatWasALongTimeAgo = correctedTime - beatSignalTime > 1500;

  if(timesInAir >= 2) {
    inAir = true;
    if(!toggledInAirStartSignal) {
      if(!lastBeatWasALongTimeAgo) {
        // Only record current jump length if jumps are regular. This way the beat signal will resume instantly when jumping again
        currentJumpLength = correctedTime - inAirStartSignalTime;
      }
      jumpLengths.push_back(currentJumpLength);
      inAirStartSignalTime = timeOfFirstInAirMeasurement;
      inAirStartSignal = true;
      strcat(concatinated, " s");

      if(serialOn) {
        //Serial.print("t");
        //Serial.println(timeOfFirstInAirMeasurement);
        Serial.println("s");
      }

      toggledBeatSignal = false;
    }
    toggledInAirStartSignal = true;

    toggledInAirEndSignal = false;
  } else if(timesNotInAir >= 2) {
    inAir = false;


    if(!toggledInAirEndSignal) {
      inAirEndSignalTime = timeOfFirstNotInAirMeasurement;
      inAirEndSignal = true;
      strcat(concatinated, " e");
      if(serialOn) {
        //Serial.print("t");
        //Serial.println(timeOfFirstInAirMeasurement);
        Serial.println("e");
      }

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
      strcat(concatinated, " a");
      if(serialOn) {
        //Serial.print("t");
        //Serial.println(inAirStartSignalTime + inAirSignalTime/2);
        Serial.println("m");
      }
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
  nextBeatTime = inAirStartSignalTime + avgJumpLength - magicJumpNumber;
  if(nextBeatTime <= correctedTime && !toggledBeatSignal) {
  //if(accelerationMagnitude > 1.3 && !toggledBeatSignal) {
    toggledBeatSignal = true;
    isStrongBeat ^= true;
    beatSignal = true;
    beatSignalTime = nextBeatTime;
    strcat(concatinated, " b");
    AppendDouble(bpm);
    if(serialOn) {
      //Serial.print("t");
      //Serial.println(nextBeatTime);
      Serial.print("b");
      Serial.println(bpm);
    }
  }

  //Serial.println(concatinated);

  strcat(concatinated, " u");
  AppendDouble(deltaTimeSeconds);
}

void UpdateIMU() {
  int i = 0;
  ReadIMU();
  //get both accel and gyro data from bmi160
  //parameter accelGyro is the pointer to store the data
  if(rslt == 0){
    // magnitude
    accelerationMagnitudeRaw = sqrt(accel[0]*accel[0] + accel[1]*accel[1] + accel[2]*accel[2])/16384.0 - 1; // make laying on the floor 0
    accelerationMagnitude = lpf.update(accelerationMagnitudeRaw, deltaTimeSeconds, 4);
    //accelerationMagnitude = accelerationMagnitudeRaw;
    if(isnan(accelerationMagnitude) || accelerationMagnitude == 0) {
      lpf = LowPassFilter(4, deltaTimeSeconds);
      // use raw data for this time
      accelerationMagnitude = accelerationMagnitudeRaw; // make laying on the floor 0
    }
  }else{
    Serial.println("err");
  }
  UpdateVariables();
}