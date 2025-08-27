
#include <DFRobot_BMI160.h>

extern bool inAir;
extern bool inAirStartSignal;
extern bool inAirMiddleSignal;
extern bool inAirEndSignal;
extern bool inAirRaw;
extern bool beatSignal;
extern bool isStrongBeat;
extern int timesNotInAir;
extern int timeInAir;
extern char concatinated[];
extern long beatSignalTime;
extern double bps;
extern double bpm;
extern bool imuWorking;
extern void InitIMU();
extern void UpdateIMU();

extern int totalSteps;
extern int stepsSinceLastSend;