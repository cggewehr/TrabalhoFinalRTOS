#include <stdbool.h>

// Global Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;

CarState_t carState;
CarParam_t carParam;
CarInterface_t carInterface;
ThreadContainer_t ThreadContainer;

typedef struct CarState{

//  Engine values
    int RPM;
    int Speed;
    int GasPctg;
    bool Ignition;

//  Transmission
    int Gear;
    bool ClutchBite;

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

    int Gear;

    int turnSignalPos;
    bool HazardLightsButton;

    int BeamButtonPos;
    int WiperButtonPos;

    bool ignitionState;

//  Output
    int tachometer;
    int speedometer;
    int gasPctg;

} CarInterface_t;

typedef struct CarParam{

//  Based on Ford Falcon XB with 351ci V8 / manual 4 speed transmission
//  More info @ https://www.uniquecarsandparts.com.au/falcon_XB_technical_specifications

    float gearRatios[5]; // Reverse, 1st, 2nd, 3rd, 4th
    float diffRatio;
    float tyreDiameter; // in inches

    int maxRPM; // Estimated
    int maxSpeed; // in mph

    int BeamIntensityPctg[3];
    int WiperIntensityPctg[5];

    int TransmissionLossPctg;

    float blinkerPeriod; // in seconds

} CarParam_t;

typedef struct ThreadContainer{

    pthread_t updateClutchBite;
    pthread_t updateSpeed;
    pthread_t updateRPM;
    pthread_t updateGear;
    pthread_t updateFuel;
    pthread_t updateBlinkers;
    pthread_t updateHeadlights;
    pthread_t updateBreaklights;
    pthread_t updateReverselights;
    pthread_t updateWipers;
    pthread_t updateInterface;
    pthread_t updateIgnition;

} ThreadContainer_t;
