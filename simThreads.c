
void* updateClutchBite(){

    while(1){

     // wait for timer

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

    while(1){

        // wait for timer

        pthread_mutex_lock(&mutex);

        carState.Speed = (0.00595) * (carState.RPM * carParam.tyreDiameter) / 2*(carParam.gearRatios[carState.Gear] * carParam.diffRatio) // speed in miles/hour

        pthread_mutex_unlock(&mutex);

    }
}

void* updateRPM(){

    while(1){

        // wait for timer

        pthread_mutex_lock(&mutex);

        if (carState.Ignition){
            carState.RPM = ( (carInterface.GasPedalPctg / 100) * carParam.maxRPM)
            carState.RPM = carState.RPM * (carParam.TransmissionLossPctg / 100);// Accounts for transmisson loss
        }
        else{
            carState.RPM = 0;
        }

        pthread_mutex_unlock(&mutex);

    }
}

void* updateGear(){

    while(1){

        // wait for timer

        pthread_mutex_lock(&mutex);

        carState.Gear = carInterface.Gear;

        pthread_mutex_unlock(&mutex);

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

        carState.GasPctg -= (carState.RPM/carParam.maxRPM)*0.001;

        pthread_mutex_unlock(&mutex);
    }
}

void* updateBlinkers(){

    // Makes a periodic signal with blinker period defined in carParam
    struct periodic_info info;
    make_periodic (carParam.blinkerPeriod * 1000000 / 2, &info); // period value is expected in microseconds

    while(1){

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

    // wait for timer

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