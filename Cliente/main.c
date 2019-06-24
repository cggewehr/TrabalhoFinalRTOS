// System libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Network socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){

    struct sockaddr_in serv_addr; // Socket struct
    struct hostent *server;
    int sockfd, portno; // Socket port
    char buffer[240], arg0compare[5];
    int i;

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

    server = gethostbyname(argv[1]);

    // Set serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy( (char*) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_len);
    serv_addr.sin_port = htons(portno);

    // Creates connection
    if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 ) {
        printf("Erro ao conectar\n");
        exit(1);
    }

    while(true){

        // Clear buffer
        bzero(buffer, 240);

        // Get command from terminal (client)
        printf("\nSimulador>");
        fgets(buffer, 240, stdin);

        // Send command to server (Simulator)
        write(sockfd, buffer, strlen(buffer));

        // Checks for "help" or "exit/quit" command
        strncpy(arg0compare, buffer, 4)
        for(i = 0 ; i < 5 ; i++){
            arg0compare[i] = tolower(arg0compare[i]);
        }

        if(strcmp(arg0compare, "exit") || strcmp(arg0compare, "quit") == 0){
            break; // Close socket and exit program
        }
        else if(strcmp(arg0compare, "help") == 0){
            printf("Comandos validos: ");
            printf("\n\t Set:        Atribui a variavel deseja o valor desejado (Set variavel valor)");
            printf("\n\t Print:      Retorna ao usuario o valor da variavel desejada (Print variavel)");
            printf("\n\t Exit/Quit:  Sai do programa (Exit/Quit)");
            printf("\n\t Help :      Exibe novamente esse menu (Help)");
        }

        // Clear buffer
        bzero(buffer, 240);

        // Get return message from simulator
        read(sockfd, buffer, 240);

        // Display return message
        printf("%s\n", buffer)
    }

    // Close socket and exit
    close(sockfd);

    return EXIT_SUCCESS;
}
