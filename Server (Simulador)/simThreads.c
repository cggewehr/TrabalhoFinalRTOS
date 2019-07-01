#include "CarroPkg.h"
#include <signal.h>
#include <stdio.h>

extern struct periodic_info{
    int sig;
    sigset_t alarm_sig;
};

static int make_periodic (int unsigned period, struct periodic_info *info){
    static int next_sig;
    int ret;
    unsigned int ns;
    unsigned int sec;
    struct sigevent sigev;
    timer_t timer_id;
    struct itimerspec itval;

    /* Initialise next_sig first time through. We can't use static
       initialisation because SIGRTMIN is a function call, not a constant */
    if (next_sig == 0)
        next_sig = SIGRTMIN;
    /* Check that we have not run out of signals */
    if (next_sig > SIGRTMAX)
        return -1;
    info->sig = next_sig;
    next_sig++;
    /* Create the signal mask that will be used in wait_period */
    sigemptyset (&(info->alarm_sig));
    sigaddset (&(info->alarm_sig), info->sig);

    /* Create a timer that will generate the signal we have chosen */
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = info->sig;
    sigev.sigev_value.sival_ptr = (void *) &timer_id;
    ret = timer_create(CLOCK_MONOTONIC, &sigev, &timer_id);
    if (ret == -1)
        return ret;

    /* Make the timer periodic */
    sec = period/1000000;
    ns = (period - (sec * 1000000)) * 1000;
    itval.it_interval.tv_sec = sec;
    itval.it_interval.tv_nsec = ns;
    itval.it_value.tv_sec = sec;
    itval.it_value.tv_nsec = ns;
    ret = timer_settime(timer_id, 0, &itval, NULL);
    return ret;
}

static void wait_period (struct periodic_info *info){
    int sig;
    sigwait (&(info->alarm_sig), &sig);
}


void* updateClutchBite(){
    
    printf("Thread updateClutchBite inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        if(carInterface.ClutchPedalPctg <= 50){
            carState.ClutchBite = true; // Engine is transmiting power
        }
        else{
            carState.ClutchBite = false; // Car is on idle
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateSpeed(){
    
    printf("Thread updateSpeed inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        if(carState.ClutchBite){ // Only sets speed to a value if clutch pedal is not pressed
            carState.Speed = (int) ( (float) ( 0.00595 * carState.RPM * (carParam.tyreDiameter/2) ) ) / ( (float) (carParam.gearRatios[carState.Gear] * carParam.diffRatio ) ); // speed in miles/hour
            
            if(carState.Gear == 0){
                carState.Speed = -carState.Speed;   
            }
            
        } else {
            carState.Speed = 0;   
        }    
            
        // Update interface
        carInterface.speedometer = carState.Speed;

        pthread_mutex_unlock(&mutex);

    }
}

void* updateRPM(){
    
    printf("Thread updateRPM inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        if (carState.Ignition == true){
            carState.RPM =  (int) ( ( (float) carInterface.GasPedalPctg / 100) * carParam.maxRPM );
            carState.RPM = (int) ( (float) carState.RPM * ( (float) ( 100 - carParam.TransmissionLossPctg ) / 100) ); // Accounts for transmisson loss
        } else {
            carState.RPM = 0;
        }

        // Update interface
        carInterface.tachometer = carState.RPM;

        pthread_mutex_unlock(&mutex);

    }
}

void* updateGear(){
    
    printf("Thread updateGear inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        carState.Gear = carInterface.Gear;

        pthread_mutex_unlock(&mutex);

    }
}

void* updateFuel(){
    
    printf("Thread updateFuel inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        // Waits for car to be turned on
        while(!carState.Ignition){
            pthread_cond_wait(&condVar, &mutex);
        }

        carState.GasPctg -= (carState.RPM/carParam.maxRPM)*0.001;
        
        // Update interface
        carInterface.gasPctg = carState.GasPctg;

        pthread_mutex_unlock(&mutex);
        
    }
}

void* updateBlinkers(){

    // Makes a periodic signal with blinker period defined in carParam divided by 2
    struct periodic_info info;
    make_periodic(carParam.blinkerPeriod * 1000000 / 2, &info); // period value is expected in microseconds
    
    printf("Thread updateBlinkers inicializada com periodo %f\n", (carParam.blinkerPeriod * 1000000 / 2));

    while(1){

        wait_period(&info);

        pthread_mutex_lock(&mutex);

        if(carInterface.HazardLightsButton == true){
            
            // Synchronizes both blinkers
            carState.LeftBlinker = !carState.LeftBlinker;
            carState.RightBlinker = carState.LeftBlinker;
            
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
        
        printf("Left Blinker: %d\nRight Blinker: %d\n", carState.LeftBlinker, carState.RightBlinker);

        pthread_mutex_unlock(&mutex);

    }
}

void* updateHeadlights(){

    printf("Thread updateHeadlights inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        carState.HeadlightsPctg = carParam.BeamIntensityPctg[carInterface.BeamButtonPos];

        if (carInterface.BeamButtonPos != 0){
            carState.Taillights = true;
        } else {
            carState.Taillights = false;
        }

        pthread_mutex_unlock(&mutex);
        
    }
}

void* updateBreaklights(){
    
    printf("Thread updateBreaklights inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        if(carInterface.BrakePedalPctg >= 10){
            carState.Breaklights = true;
        } else {
            carState.Breaklights = false;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateReverselights(){
    
    printf("Thread updateReverseLights inicializada\n");

    while(1){

        pthread_mutex_lock(&mutex);

        if(carState.Gear == 0 && carState.ClutchBite && carInterface.GasPedalPctg > 5){
            carState.ReverseLight = true;
        } else {
            carState.ReverseLight = false;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateWipers(){
    
    // Makes a periodic signal with 1 sec period
    struct periodic_info info;
    make_periodic(1000000, &info); // period value is expected in microseconds
    
    printf("Thread updateWipers inicializada com periodo %d\n", 1000000);
    
    while(1){
        
        wait_period(&info);

        pthread_mutex_lock(&mutex);

        carState.wiperSpeed = carParam.WiperIntensityPctg[carInterface.WiperButtonPos];
        
        printf("Wiper Speed: %d\n", carState.wiperSpeed);

        pthread_mutex_unlock(&mutex);

    }
}

void* updateInterface(){

    // Makes a periodic signal with 1 sec period
    struct periodic_info info;
    make_periodic(1000000, &info); // period value is expected in microseconds
    
    printf("Thread updateBlinkers inicializada com periodo %d\n", 1000000);

    while(1){
        
        wait_period(&info);

        pthread_mutex_lock(&mutex);

        carInterface.speedometer = carState.Speed;
        carInterface.tachometer = carState.RPM;
        
        printf("Speed: %d\nRPM: %d\n", carState.Speed, carState.RPM);

        pthread_mutex_unlock(&mutex);
        
    }
}

void* updateIgnition(){
    
    printf("Thread updateIgnition inicializada\n");

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
