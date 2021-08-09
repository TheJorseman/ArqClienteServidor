# Proyecto Arquitectura ClienteServidor

## Objetivo

La intención del proyecto es que se desarrollará un cliente servidor de BD básico, programado en lenguaje C. 
El cliente envía un comando al servidor, éste realiza la operación correspondiente y envía la respuesta al cliente.



## Comandos

Por facilidad los comandos no tendrán la sintaxis completa de SQL. Los comandos que el cliente puede realisar son 3:

1. “INSERT numcta apellido_paterno apellido_materno nombre(s)”: donde numcta es el número de cuenta del alumno a insertar en la BD, seguido del apellido paterno, el apellido materno y el nombre o nombres.

```sh
INSERT 314009221 Vicente Garcia Martinez
```

- Al recibir el comando “INSERT”, el servidor escribe en el archivo el nombre completo del alumno y envia un mensaje de éxito o error como respuesta al cliente, para que éste lo despliegue en su pantalla.

2. “SELECT numcta”: donde numcta es el número de cuenta del alumno a consultar. 

```sh
SELECT 314009221
```

- Al recibir el comando “SELECT”, se lee el archivo y envia el contenido como respuesta al cliente, para que éste lo despliegue en su pantalla, en caso de no encontrar el registro tambien recibe una respuesta.

3. “DELETE numcta”: donde numcta es el número de cuenta del alumno a consultar. 

- Al recibir el comando "DELETE”, se elimina el archivo correspondiente y envia como respuesta al cliente un mensaje de éxito, para que éste lo despliegue en su pantalla, en caso de no encontrar el registro tambien recibe una respuesta.

```sh
DELETE 314009221
```

4. “HELP”. 

```sh
HELP
```

- Al recibir el comando "HELP”, se muestra una breve guía con los comandos disponibles y la sintaxis que lleva cada uno de ellos.




## Como Ejecutar

Para ejecutar es necesario tener instalado un compilador de código en C, a continuación se muestran los comandos a seguir con el cpomilador GCC.


- Compilar código del servidor.

```sh
gcc server.c -o server
```

- Compilar código del cliente.

```sh
gcc client.c -o client
```

- Ejecutar código del servidor.

```sh
./server
```

- Ejecutar código del cliente.
```sh
./cliente
```

## Capturas de Resultados

- Captura Servidor

![alt text](https://cdn.discordapp.com/attachments/823392223432802344/874138464709402665/unknown.png)

- Captura Cliente

![alt text](https://cdn.discordapp.com/attachments/823392223432802344/874138370123632660/unknown.png)
