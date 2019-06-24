// System libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// Network socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>      

// Program header files
#include "carroPkg.h"
#include "timer.h"
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

void argparse(char buffer[160], char* args[3]){

    int i = 0;
    const char espaço[2] = " ";
    char* argPtr;

    // Convert all chars to lowercase
    for(i = 0 ; i < 240 ; i++){
        if(buffer[i] == '\n'){
            buffer[i] = ' '; // If given command has no arguments, inserts ' ' separator so strtok() can do its thing
        }
        buffer[i] = tolower(buffer[i]);
    }

    // Set arguments (finds ' ' between command and its arguments)
    for(i = 0 ; i < 3 ; i++){
        argPtr = strtok(buffer, espaço);
        args[i] = argPtr;
    }

}

int main(int argc, char *argv[]){

    ThreadContainer_t ThreadContainer; // Struct to save thread ID's
    sigset_t alarm_sig; // sigset for periodic threads

    struct sockaddr_in serv_addr, cli_addr; // Socket struct
    socklen_t clilen; 
    int newsockfd, sockfd, portno; // Socket port
    char buffer[240], printReturn[80];
    char* args[4];

    args[0] = malloc(80);
    args[1] = malloc(80);
    args[2] = malloc(80);
    args[3] = NULL;

    if ( argc < 2 ) {
        printf("Erro, porta nao definida\n");
        exit(1);
    }

    // Cria socket TCP IPv4 e define porta a ser utilizada
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    portno = atoi(argv[1]);

    if ( sockfd < 0 ) {
        printf("Erro abrindo o socket\n");
        exit(1);
    }

    // Set serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind socket to port
    if ( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) {
        printf("Erro fazendo bind\n");
        exit(1);
    }

    // Creates connection
    listen(sockfd, 1);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd == -1){
        printf("Erro aceitando conexao\n");
    }

    // Define timers
    sigemptyset (&alarm_sig);                      // Block all real time signals so they can be used for the timers.
    for (i = SIGRTMIN; i <= SIGRTMAX; i++)         // Note: this has to be done in main() before any threads are created
        sigaddset (&alarm_sig, i);                 // so they all inherit the same mask. Doing it later is subject to
    sigprocmask (SIG_BLOCK, &alarm_sig, NULL);     // race conditions

    // Initialize simulation
    setParam();
    resetState();

    // Create simulation threads
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

    while(true){

        // Clear buffer
        bzero(buffer, 240);

        // Get command from terminal (client)
        read(newsockfd, buffer, 240);

        // Organize command into array
        argparse(buffer, args);

        // Locks mutex to execute given command
        pthread_mutex_lock(&mutex);

        if(strcmp(args[0], "set") == 0){

            // Sets return message
            strcpy(buffer, "Variavel ");
            strcat(buffer, args[1]);
            strcat(buffer, " setada para o valor ");
            strcat(buffer, args[2]);

            if(strcmp(args[1], "gaspedalpctg") == 0){
                carInterface.GasPedalPctg = atoi(args[3]);
            }
            else if(strcmp(args[1], "brakepedalpctg") == 0){
                carInterface.BrakePedalPctg = atoi(args[3]);
            }
            else if(strcmp(args[1], "clutchpedalpctg") == 0){
                carInterface.ClutchPedalPctg = atoi(args[3]);
            }
            else if(strcmp(args[1], "gear") == 0){
                carInterface.Gear = atoi(args[3]);
            }
            else if(strcmp(args[1], "turnsignalpos") == 0){
                carInterface.turnSignalPos = atoi(args[3]);
            }
            else if(strcmp(args[1], "hazardlightsbutton") == 0){
                if(atoi(args[2]) == 1 || strcmp(args[2], "on")){
                    carInterface.HazardLightsButton = true;
                }
                else if(atoi(args[2]) == 0 || strcmp(args[2], "off")){
                    carInterface.HazardLightsButton = false;
                }
                else{
                    strcpy(buffer, "Segundo argumento deve ser 1, 0, \"on\" ou \"off\" "); // Overrides message set before args[1] comparisons
                }
            }
            else if(strcmp(args[1], "beambuttonpos") == 0){
                carInterface.BeamButtonPos = atoi(args[2]);
            }
            else if(strcmp(args[1], "wiperbuttonpos") == 0){
                carInterface.WiperButtonPos = atoi(args[3]);
            }
            else if(strcmp(args[1], "ignition") == 0){
                if(atoi(args[2]) == 1 || strcmp(args[2], "on")){
                    carInterface.ignitionState = true;
                }
                else if(atoi(args[2]) == 0 || strcmp(args[2], "off")){
                    carInterface.ignitionState = false;
                }
                else{
                    strcpy(buffer, "Segundo argumento deve ser 1, 0, \"on\" ou \"off\" "); // Overrides message set before args[1] comparisons
                }
            }
            else{
                strcpy(buffer, "Variavel alvo nao reconhecida. Variaveis validas: \n\t GasPedalPctg \n\t BrakePedalPctg \n\t ClutchPedalPctg \n\t Gear \n\t TurnSignalPos \n\t HazardLightsButton \n\t BeamButtonPos \n\t WiperButtonPos \n\t Ignition");
            }
        }

        else if(strcmp(args[0], "print") == 0){

            strcpy(buffer, "O valor da variavel ");
            strcat(buffer, args[1]);
            strcat(buffer, " e: ");
           
            if(strcmp(args[1], "gaspedalpctg") == 0){
                itoa(carInterface.GasPedalPctg, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "brakepedalpctg") == 0){
                itoa(carInterface.BrakePedalPctg, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "clutchpedalpctg") == 0){
                itoa(carInterface.ClutchPedalPctg, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "gear") == 0){
                itoa(carInterface.Gear, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "turnsignalpos") == 0){
                itoa(carInterface.TurnSignalPos, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "hazardlightsbutton") == 0){
                itoa(carInterface.HazardLightsButton, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "beambuttonpos") == 0){
                itoa(carInterface.BeamButtonPos, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "wiperbuttonpos") == 0){
                itoa(carInterface.WiperButtonPos, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "ignition") == 0){
                itoa(carInterface.ignition, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "tachometer") == 0){
                itoa(carInterface.tachometer, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "speedometer") == 0){
                itoa(carInterface.speedometer, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else if(strcmp(args[1], "gaspctg") == 0){
                itoa(carInterface.gaspctg, printReturn, 10);
                strcat(buffer, printReturn);
            }
            else{
                strcpy(buffer, "Variavel alvo nao reconhecida. Variaveis validas: \n\t GasPedalPctg \n\t BrakePedalPctg \n\t ClutchPedalPctg \n\t Gear \n\t TurnSignalPos \n\t HazardLightsButton \n\t BeamButtonPos \n\t WiperButtonPos \n\t Ignition");
            }
        }

        else if(strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0){

            // Close sockets and exit
            close(newsockfd);
            close(sockfd);
            exit(EXIT_SUCCESS);
        }

        else if(strcmp(args[0], "help") == 0){
            strcpy(buffer, " \n");
            // printf("Comandos validos: ");
            // printf("\n\t Set:        Atribui a variavel deseja o valor desejado (set variavel valor)");
            // printf("\n\t Print:      Retorna ao usuario o valor da variavel desejada (print variavel)");
            // printf("\n\t Exit\\Quit: Sai do programa (Exit\\Quit)");
            // printf("\n\t Help :      Exibe novamente esse menu (Help)");
        }

        else{
            strcpy(buffer, "Insira um comando válido (Insira \"help\" para saber mais)");
        }

        // Send back to client a return message
        write(newsockfd, buffer, strlen(buffer));

        // Unlocks mutex and waits for next command
        pthread_mutex_unlock(&mutex);

    }

    return EXIT_FAILURE;
}
