// System libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

//  Program header files
#include "carroPkg.h"
#include "simThreads.h"

void setParam(){

    //carParam.gearRatios[5] = {2.56, 2.82, 1.84, 1.32, 1.0}; // Reverse, 1st, 2nd, 3rd, 4th
    carParam.gearRatios[0] = 2.56; // Reverse
    carParam.gearRatios[1] = 2.82; // 1st
    carParam.gearRatios[2] = 1.84; // 2nd
    carParam.gearRatios[3] = 1.32; // 3nd
    carParam.gearRatios[4] = 1.00; // 4th
    carParam.diffRatio = 3.0;
    carParam.tyreDiameter = 34.8333333334; // in inches

    carParam.maxRPM = 6000;  // Estimated
    carParam.maxSpeed = 125; // in mph

    //carParam.BeamIntensityPctg[3] = {0, 50, 100};
    carParam.BeamIntensityPctg[0] = 0;
    carParam.BeamIntensityPctg[1] = 50;
    carParam.BeamIntensityPctg[2] = 100;

    //carParam.WiperIntensityPctg[5] = {0, 25, 50, 75, 100};
    carParam.WiperIntensityPctg[0] = 0;
    carParam.WiperIntensityPctg[1] = 25;
    carParam.WiperIntensityPctg[2] = 50;
    carParam.WiperIntensityPctg[3] = 75;
    carParam.WiperIntensityPctg[4] = 100;

    carParam.TransmissionLossPctg = 5;
}

void resetState(){

    //  Engine
    carState.RPM = 0;
    carState.Speed = 0;
    carState.Gear = 0;
    carState.GasPctg = 100;
    carState.ClutchBite = false;
    carState.Ignition = false;

    //  Lights
    carState.LeftBlinker = false;
    carState.RightBlinker = false;
    carState.HeadlightsPctg = 0;
    carState.Taillights = false;
    carState.Breaklights = false;
    carState.ReverseLight = false;

    //  Wipers
    carState.wiperSpeed = 0;
}

int main(){

    ThreadContainer_t ThreadContainer;

    // Inicializa Simulação
    setParam();
    resetState();

    // Define timers
    printf ("Periodic threads using POSIX timers\n");

    /* Block all real time signals so they can be used for the timers.
       Note: this has to be done in main() before any threads are created
       so they all inherit the same mask. Doing it later is subject to
       race conditions */
    sigemptyset (&alarm_sig);
    for (i = SIGRTMIN; i <= SIGRTMAX; i++)
        sigaddset (&alarm_sig, i);
    sigprocmask (SIG_BLOCK, &alarm_sig, NULL);
    

    // Criar threads
    pthread_create( &(ThreadContainer.updateClutchBite) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateSpeed) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateRPM) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateGear) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateFuel) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateBlinkers) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateHeadlights) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateBreaklights) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateReverselights) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateWipers) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateInterface) , NULL, teste, NULL);
    pthread_create( &(ThreadContainer.updateIgnition) , NULL, teste, NULL);

    // Criar interface de comunicação 

    pthread_exit(NULL);

    return 0;
}
