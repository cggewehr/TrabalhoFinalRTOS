#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;

CarState_t carState;
CarParam_t carParam;
CarInterface_t carInterface;

void* updateClutchBite(){

    while(1){

        pthread_mutex_lock(&mutex);

        if(carInterface.ClutchPedalPctg <= 50){
            carState.ClutchBite = true; // Engine is transmiting power
        }
        else{
            carState.ClutchBite = false; // Car is on idle
        }

        pthread_mutex_lock(&mutex);

    }
}

void* updateFuel(){

    while(1){

        // Wait for timer

        pthread_mutex_lock(&mutex);

        // Waits for car to be turned on
        while(!carState.Ignition){
            pthread_cond_wait(&condVar, &mutex);
        }

        carState.GasPctg -= (carState.RPM/carParam.maxRPM)*0.15;

        pthread_mutex_unlock(&mutex);
    }
}

void* updateBlinkers(){

    while(1){

        // Wait for timer

        pthread_mutex_lock(&mutex);

        if(carInterface.HazardLightsButton){
            carState.LeftBlinker = !carState.LeftBlinker;
            carState.RightBlinker = !carState.RightBlinker;
        }
        else if(carInterface.turnSignalPos == -1){ // left blinker
            carState.LeftBlinker = !carState.LeftBlinker;
            carState.RightBlinker = false;
        }
        else if(carInterface.turnSignalPos == 1){ // right blinker
            carState.LeftBlinker = false;
            carState.RightBlinker = !carState.RightBlinker;
        }
        else if(carInterface.turnSignalPos == 0){
            carState.LeftBlinker = false;
            carState.RightBlinker = false;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateHeadlights(){

    while(1){

        // wait for timer

        pthread_mutex_lock(&mutex);

        carState.HeadlightsPctg = carParam.BeamIntensityPctg[carInterface.BeamButtonPos];

        if (carInterface.BeamButtonPos != 0){
            carState.Taillights = true;
        }
        else{
            carState.Taillights = false;
        }

        pthread_mutex_unlock(&mutex);
    }
}

void* updateBreaklights(){

    while(1){

        // wait for timer

        pthread_mutex_lock(&mutex);

        if(carInterface.BrakePedalPctg >= 10){
            carState.Breaklights = true;
        }
        else{
            carState.Breaklights = false;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateReverselights(){

    while(1){

        pthread_mutex_lock(&mutex);

        if(carState.Gear == 0 && carState.ClutchBite && carInterface.GasPedalPctg > 5){
            carState.ReverseLight = true;
        }
        else{
            carState.ReverseLight = false;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateWipers(){

    // wait for timer

    while(1){

        pthread_mutex_lock(&mutex);

        carState.wiperSpeed = carParam.WiperIntensityPctg[carInterface.WiperButtonPos];

        pthread_mutex_unlock(&mutex);

    }
}

void* updateInterface(){

    // wait for timer

    while(1){

        pthread_mutex_lock(&mutex);

        carInterface.speedometer = carState.Speed;
        carInterface.tachometer = carState.RPM;

        pthread_mutex_unlock(&mutex);
    }
}

void* updateIgnition(){

    // wait for timer

    while(1){

        pthread_mutex_lock(&mutex);

        if(carInterface.ignitionState){
            pthread_cond_signal(&condVar);
            carState.Ignition = true;
        }
        else{
            carState.Ignition = false;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void setParam(){

    carParam.gearRatios[5] = {2.56, 2.82, 1.84, 1.32, 1.0}; // Reverse, 1st, 2nd, 3rd, 4th
    carParam.diffRatio = 3.0;
    carParam.tyreDiameter = 34.8333333334; // in inches

    carParam.maxRPM = 6000; // Estimated
    carParam.maxSpeed = 125; // in mph

    carParam.BeamIntensityPctg[3] = {0, 50, 100};
    carParam.WiperIntensityPctg[5] = {0, 25, 50, 75, 100};
}

void resetState(){

    carState.RPM = 0;
    carState.Speed = 0;
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

    setParam();
    resetState();


    pthread_exit(NULL);

    return 0;
}
