//Librerías utilizadas
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

//El puerto del usuario al que se va a conectar
#define MYPORT 3490

//Se define que se puede tener hasta 10 conexiones en cola
#define BACKLOG 10
#define MAXDATASIZE 300

//Se comprueba que no haya procesos hijos
void sigchld_handler(int s){
  while(wait(NULL) > 0);
}

//Función principal
int main(int argc, char *argv[ ]){
  // Se escuchara mediante sock_fd, se hara una nueva conexión con new_fd
  int sockfd, new_fd;

  // La información de mi dirección
  struct sockaddr_in my_addr;

  // La información de la dirección de los conectores
  struct sockaddr_in their_addr;
  int sin_size;
  struct sigaction sa;
  int yes = 1;
  int numbytes;
  char buf[MAXDATASIZE];

  //Si falla la creación del socket manda mensaje de error.
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Server-socket() error lol!");
    exit(1);
  }
  else
    printf("Server-socket() sockfd is OK...\n"); //Manda mensaje exitoso si se crea correctamente
  //Se establece la configuración del nivel del socket, el uso de las direcciones, el puntero y tamaño de los datos.
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
    perror("Server-setsockopt() error lol!"); //Mensaje de error
    exit(1);
  }
  else
    printf("Server-setsockopt is OK...\n"); //Mensaje de éxito

  // Orden de los bytes del host
  my_addr.sin_family = AF_INET;

  // Orden de los bytes de la red
  my_addr.sin_port = htons(MYPORT);

  // Regresa la IP que se le dió al servidor
  my_addr.sin_addr.s_addr = INADDR_ANY;
  printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);

  /* Cero para el resto de la estructura */memset(&(my_addr.sin_zero), '\0', 8);
  //Se asocia la dirección y tamaño al socket
  if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
    perror("Server-bind() error");//Mensaje de error
    exit(1);
  }
  else
    printf("Server-bind() is OK...\n");//Mensaje de éxito
  //Comprobamos que el servidor esta listo para recibir peticiones
  if(listen(sockfd, BACKLOG) == -1){
    perror("Server-listen() error");//Mensaje de error
    exit(1);
  }

  printf("Server-listen() is OK...Listening...\n");//Mensaje de éxito
  
  //Limpia los procesos muertos
  sa.sa_handler = sigchld_handler;
  //Limpia el conjunto de señales
  sigemptyset(&sa.sa_mask);
  //Se define el comportamiento de la primitiva cuando es interrumpida por una señal.
  sa.sa_flags = SA_RESTART;
  //Se estable la acción para la señal SIGCHLD
  if(sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("Server-sigaction() error");//Mensaje de error
    exit(1);
  }
  else
    printf("Server-sigaction() is OK...\n");//Mensaje de éxito

  /* accept() loop */
  while(1){
    sin_size = sizeof(struct sockaddr_in);
    //Se comprueba si se acepta la conexión con el cliente que la solicita
    if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1){
      perror("Server-accept() error");//Mensaje de error.
      continue;
    }
    else
      printf("Server-accept() is OK...\n");//Mensaje de exito.
    printf("Server-new socket, new_fd is OK...\n");
    printf("Server: Got connection from %s\n", inet_ntoa(their_addr.sin_addr)); //Muestra la dirección del cliente

    /* this is the child process */
    
    if(!fork()){
      /* child doesnt need the listener */
      close(sockfd); //Cierra el socket
      if(send(new_fd, "This is a test string from server!\n", 37, 0) == -1) //Es lo que se le envia al cliente
        perror("Server-send() error lol!");
      close(new_fd);
      exit(0);
    }
    else
      printf("Server-send is OK...!\n");

    //Se comprueba lo que se esta recibiendo del cliente
    printf("strcmp %d\n", strcmp(buf,"exit"));
    while(strcmp(buf,"exit") != 0){
      if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
        perror("recv()");
        exit(1);
      }
      else
        printf("Server-The recv() is OK...\n");
      buf[numbytes] = '\0';
      printf("Server-Received: %s\n", buf); //Muestra lo recibido por el server.
    }

    close(new_fd);
    printf("Server-new socket, new_fd closed successfully...\n");
    
    /* parent doesnt need this */

  }
  return 0;
}
