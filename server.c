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
#define NUMCTALEN 9
#define MAXSTR 256
#define MAXARGS 10
#define NARGSSHIFT 2

//Función para insertar un registro
//_insert(numero de cuenta, argumentos (nombre completo), numero de argumentos)
//Regresa un apuntador char con la respuesta
char * _insert(char* numcta, char** args, int nargs){
  char * data_concat = (char *)malloc(sizeof(char)*MAXDATASIZE);
  char * response = (char *)malloc(sizeof(char)*15);
  FILE *archivo;
  int i;
  for (i=NARGSSHIFT;i<NARGSSHIFT+ nargs;i++){
    strcat(data_concat, " ");
    strcat(data_concat, args[i]);
  }
  archivo = fopen(numcta,"w+");
  fputs(data_concat, archivo);
  fclose(archivo);
  strcpy(response, "Insert Exitoso\n");
  return response;
}

//Función para consultar un registro
//_select(numero de cuenta)
//Regresa un apuntador char con la respuesta
char * _select(char* numcta){
  char * response = (char *)malloc(sizeof(char)*MAXDATASIZE);
  FILE *archivo;
  archivo = fopen(numcta,"r");
  if (archivo != NULL){
    fgets(response, MAXDATASIZE, archivo);
    fclose(archivo);
  }
  if (archivo == NULL){
    strcpy(response, "El Registro no existe.\n");
  }
  return response;
}

//Función para eliminar un registro
//_delete(numero de cuenta)
//Regresa apuntador char con respuesta
char * _delete(char* numcta){
  char * response = (char *)malloc(sizeof(char)*MAXDATASIZE);
  if (remove(numcta) != 0){
    strcpy(response, "El Registro no existe.\n");
  }else
    strcpy(response, "Se elimino el Registro.");
  return response;
}

char * _help(){
	char * response = (char *)malloc(sizeof(char)*MAXDATASIZE);
	strcpy(response, "Se admiten los siguientes comandos:\ninsert numero_de_cuenta nombre(s) apellidos\nselect numero_de_cuenta\ndelete numero_de_cuenta\nexit");
}

//Función para convertir los caracteres de una cadena a minúsculas
//lower_str(cadena a convertir)
//Regresa un apuntador char a la nueva cadena
char * lower_str(char* str) {
  char * option = (char *)malloc(sizeof(char)*MAXDATASIZE);
  for(int i = 0; str[i]; i++){
    option[i] = tolower(str[i]);
  }
  return option;
}

//Función para obtener el número de argumentos
//get_args_len(arreglo de cadenas)
//Devuelve el número de argumentos
int get_args_len(char **datas){
  int j = 0;
  for (int i=NARGSSHIFT;i<MAXARGS;i++){
    if (strcmp(datas[i],"") != 0){
      j++;
    }
  }
  return j;
}

//Función para llevar el control de las instrucciones
//handler(arreglo de strings)
//Devuelve la respuesta según la acción ejecutada 
char * handler(char **datas){
  char * option = lower_str(datas[0]);
  //char * response = (char *)malloc(sizeof(char)*MAXDATASIZE);
  char * response;
  char * numcta = (char *)malloc(sizeof(char)*NUMCTALEN);
  int nargs = get_args_len(datas);

  if (strcmp(option,"insert")==0){
    response = _insert(datas[1], datas, nargs);
  }else if (strcmp(option,"select")==0){
    response = _select(datas[1]);
  }else if (strcmp(option,"delete")==0){
    response = _delete(datas[1]);
  }else if (strcmp(option,"help")==0){
    response = _help();
  }else{
    strcpy(response, "Comando no encontrado");
  }
  return response;

}

//Función para hacer el spĺit de la cadena recibida
//split_str(cadena ingresada por usuario)
//Devuelve un arreglo de cadenas
char ** split_str(char* buf){
   char ** datas=(char **)malloc(sizeof(char *)*MAXARGS);
   char * token = strtok(buf, " ");
   int i = 0;
   for (i=0;i<MAXARGS;i++){
     datas[i]=(char *)malloc(sizeof(char)*MAXDATASIZE);
   }
   i=0;
   while( token != NULL ) {
      strcpy(datas[i], token);
      token = strtok(NULL, " ");
      i++;
   }
   return datas;
}


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

    char ** datas;
    char * response;
    //Se comprueba lo que se esta recibiendo del cliente
    char * buf_v;
	//se compara si se recibe un exit, palabra clave para la desconexión
    while(strcmp(buf,"exit") != 0){
      if((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1){
        perror("recv()");
        exit(1);
      }
      else
        printf("Server-The recv() is OK...\n");
      buf[numbytes] = '\0';
      printf(">> %s\n", buf); //Muestra lo recibido por el server.
      //Se convierte a minúsculas el texto recibido
	  buf_v = lower_str(buf);
	  //Se compara el comando para ver si coincide con exit
      if (strcmp(buf_v, "exit") == 0){
        break;
      }
	  //arreglo de strings separados por espacios
      datas = split_str(buf);
	  //se manda al handler para ejecutar una acción
      response = handler(datas);
      if(send(new_fd, response, strlen(response), 0) == -1){
        perror("send()");
      }else{
        printf("Respuesta enviada: %s\n", response);
      }
    }
	//para que termine la ejecución al hacer una nueva conexión
	strcpy(buf,"NonexD");
    close(new_fd);
    printf("Server-new socket, new_fd closed successfully...\n");

  }
  return 0;
}
