//Librerías utilizadas
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// the port client will be connecting to
#define PORT 3490
// max number of bytes we can get at once
#define MAXDATASIZE 300

int main(int argc, char *argv[]){
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct hostent *he;

  // connectors address information
  struct sockaddr_in their_addr;
  // if no command line argument supplied
  if(argc != 2){
    fprintf(stderr, "Client-Usage: %s host_servidor\n", argv[0]);

    // just exit
    exit(1);
  }

  // get the host info
  //Comprobando que se obtenga la información del servidor
  if((he=gethostbyname(argv[1])) == NULL){
    perror("gethostbyname()");
    exit(1);
  }
  else
    printf("Client-The remote host is: %s\n", argv[1]); //Indica a que servidor estas conectado

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){ //Crea el socket de internet
    perror("socket()");
    exit(1);
  }
  else 
    printf("Client-The socket() sockfd is OK...\n"); //Mensaje de que se creo correctamente el socket

  // host byte order
  their_addr.sin_family = AF_INET;
  // short, network byte order
  printf("Server-Using %s and port %d...\n", argv[1], PORT); //Indica IP y puerto usados para la conexión del servidor
  their_addr.sin_port = htons(PORT); //Covierte a short
  their_addr.sin_addr = *((struct in_addr *)he->h_addr); //Dirección del servidor

  // zero the rest of the struct
  memset(&(their_addr.sin_zero), '\0', 8);
  //Se establece la conexión con el servidor con los datos obtenidos anteriormente
  if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
    perror("connect()");
    exit(1);
  }
  else{
    printf("Client-The connect() is OK...\n");
    int max_val = 255;
    char option[max_val];
    while(strcmp(option,"exit\n") != 0){
      //Codigo para enviar una cadena
      printf(">>");
      fgets(option, max_val, stdin);
      if(send(sockfd, option, strlen(option)-1, 0) == -1){
        perror("Server-send() error lol!");
      }
      else
        //printf("Client-The send() is OK...\n");

      //Se comprueba lo que se esta recibiendo del servidor
      if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1){
        perror("recv()");
        exit(1);
      }
      else{
        buf[numbytes] = '\0';
        printf(">> %s\n", buf);
      }
    }
  }
  printf("Client-Closing sockfd\n");
  close(sockfd);
  return 0;
}
