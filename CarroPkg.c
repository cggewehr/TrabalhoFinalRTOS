#include <stdbool.h>

typedef struct CarState{

//  Engine values
    int RPM;
    int Speed;
    int GasPctg;
    bool ClutchBite;
    bool Ignition;

//  Lights
    bool LeftBlinker;
    bool RightBlinker;
    int HeadlightsPctg;
    bool Taillights;
    bool Breaklights;
    bool ReverseLight;

//  Wipers
    int wiperSpeed;

} CarState_t;

typedef struct CarInterface{

//  Input
    int GasPedalPctg;
    int BrakePedalPctg;
    int ClutchPedalPctg;

    int turnSignalPos;
    bool HazardLightsButton;

    int BeamButtonPos;
    int WiperButtonPos;

    bool ignitionState;

//  Output
    int tachometer;
    int speedometer;

}CarInterface_t;

typedef struct CarParam{

//  Based on Ford Falcon XB with 351ci V8 / manual 4 speed transmission
//  More info @ https://www.uniquecarsandparts.com.au/falcon_XB_technical_specifications

    float gearRatios[5] = {2.56, 2.82, 1.84, 1.32, 1.0}; // Reverse, 1st, 2nd, 3rd, 4th
    float diffRatio = 3.0;
    float tyreDiameter = 34.8333333334; // in inches

    int maxRPM = 6000; // Estimated
    int maxSpeed = 125; // in mph

    int BeamIntensityPctg[3] = {0, 50, 100};
    int WiperIntensityPctg[5] = {0, 25, 50, 75, 100};

} CarParam_t;
