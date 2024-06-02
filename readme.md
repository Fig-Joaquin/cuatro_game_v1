# Juego Conecta Cuatro
Software desarrollado por Joaquín Sanchez e Ignacio Muñoz.

## Introducción

El juego consiste en una recreación del juego Conecta Cuatro. En pocas palabras, dos jugadores deben tirar fichas, 1 ficha por turno y cada jugador tiene 21 fichas como máximo, si conectan 4 fichas de forma horizontal, vertical u horizontal se dicta el ganador del juego.

La recreación del juego, se desarrolló en C++ a través de sockets con la finalidad de que exista un cliente y un servidor. El cliente se conecta a través del servidor mediante la IP y un Puerto para crear una conexión y así el cliente pueda jugar contra el servidor. El cliente tiene por defecto la ficha de nombre C y el servidor tiene por defecto la ficha de nombre S.

## Aspectos técnicos del juego

**1. Cliente-Servidor:** Enfréntate contra el servidor en una partida multijugador a través de una conexión con el servidor.

* El juego se desarrolló en C++
+ Se utilizaron sockets para la comunicación entre el cliente y el servidor sobre el protocolo TCP.
* Juego orientado a objetos.
* La lógica del juego está encapsulada en fourmodel.h y fourmodel.cpp.
* game.cpp contiene el bucle principal del juego para una versión local.
* Se implementó multithreading utilizando <thread> para manejar múltiples clientes concurrentemente en el servidor.
* Cada cliente se maneja en un hilo separado.


## Estructura del juego mediante los siguientes archivos

**SRC:**

* **client.cpp:** Controla la lógica del cliente, desde la conexión al servidor hasta la recepción y envío de mensajes.
* **fourmodel.cpp:** Define la lógica del juego en sí, incluyendo la representación del tablero, la colocación de fichas, la verificación de victorias y el cambio de turno.
* **fourmodel.h:** Sirve como cabecera para `fourmodel.cpp`, declarando las clases y funciones que forman el modelo del juego.
* **game.cpp:** Implementa la lógica principal del juego, inicializando la partida, gestionando los turnos, mostrando el tablero y detectando al ganador.
* **Makefile:** Contiene las instrucciones para compilar el proyecto de forma eficiente, simplificando el proceso de desarrollo.
* **server.cpp:** Toma las riendas del servidor, creando el socket de escucha, aceptando conexiones de clientes, gestionando hilos individuales para cada jugador y manejando el envío y recepción de mensajes.

## Server.cpp
Este es el servidor del juego. Se puede comunicar con diferentes clientes, para así crear varios juegos de manera simultánea. 
* El código utiliza sockets de POSIX para establecer y gestionar la comunicación en red entre el servidor y múltiples clientes. Esto es esencial para juegos multijugador en red, permitiendo que los clientes y el servidor intercambien datos de manera eficiente y en tiempo real.
* El uso de hilos permite que cada cliente juegue su propia partida contra la CPU sin interferir con otros clientes. Esto asegura que el servidor pueda manejar múltiples conexiones simultáneamente.
* Características.
1. Almacena la información del cliente, como el socket y la dirección del cliente.
2. Envía el estado actual del tablero al cliente, representando el tablero como una cadena de texto.
3. Maneja la lógica del juego entre un cliente y la CPU.
4. Decide aleatoriamente quién empieza el juego.
5. Alterna los turnos entre el cliente y la CPU.
6. Verifica las entradas del cliente para asegurarse de que son válidas.
7. Actualiza el estado del tablero y verifica si hay un ganador o si se alcanza un empate.
8. Envía mensajes al cliente sobre el estado del juego y el resultado.
9. Maneja la conexión inicial con el cliente.
10. Espera el mensaje "start" del cliente para iniciar el juego.
11. Crea un hilo separado para manejar el juego utilizando la función jugarContraCPU.
12. Configura el servidor, crea el socket de escucha y lo enlaza a una dirección y puerto.
13. Pone el servidor en modo de espera para aceptar nuevas conexiones de clientes.
14. Para cada nueva conexión, crea una estructura ClienteInfo y un nuevo hilo para manejar el juego del cliente de forma independiente.
## Client.cpp
Lógica del código.
1. Verifica que se proporcionen correctamente la dirección IP del servidor y el puerto a través de los argumentos de línea de comandos.
1. Conexión al Servidor:
* Crea un socket para la comunicación.
* Configura la dirección del servidor y convierte la dirección IP en un formato de red.
* Establece una conexión con el servidor.
1. Interacción con el Usuario:
* Solicita al usuario que escriba "start" para comenzar el juego o "Q" para salir.
* Envía el comando al servidor para iniciar el juego o termina si el usuario elige salir.
* Existe un manejo de errores con las entradas.
1. Bucle Principal del Juego:
* Recibe y muestra mensajes del servidor, incluidos el estado del tablero y mensajes de turno.
* Si es el turno del cliente, solicita al usuario que ingrese una columna válida (1-7) o "Q" para salir, y envía la elección al servidor.
* Verifica si el juego ha terminado (ganar, perder, empate) y muestra el mensaje correspondiente antes de salir del bucle.
1. Cierre de la Conexión:
* Cierra el socket y finaliza el programa cuando el juego termina o si la conexión se pierde.

## Server.cpp
## Requisitos previos para la compilación y ejecución
1. Tener instalado el gcc, g++ o por defecto el lenguaje C y C++.
1. Sistema operativo Linux. (No testeado en otros sistemas)
1. Si el servidor se abre en el puerto 8080, el puerto debe estar liberado.
1. En el caso de que el servidor se ejecute correctamente y el puerto sea 8080, el cliente debe tener el mismo puerto de conexión, en este caso 8080.

## Compilación y ejecución

Para preparar el juego para su ejecución, sigue estos sencillos pasos:

1. Abre una terminal y clona el repositorio. Puedes bajar el zip también. (Opcional)
2. Navega hasta la carpeta src mediante la terminal con los comandos cd.
3. Ejecuta el comando `make` mediante la terminal para compilar el proyecto (Debes estar dentro la carpeta src). Esto generará los ejecutables `client` y `server` en la carpeta `src`.
```bash
make
``` 

1. ¡Sin el paso anterior no se podrá ejecutar el servidor ni el cliente!
## **Ejecutando el servidor:**

1. Abre una terminal y navega hasta el directorio `src` si es qué ya no estás en la carpeta.
2. Ejecuta el comando `./server <puerto>`, reemplazando `<puerto>` por el puerto que deseas utilizar para las conexiones. (El puerto puede ser variable)
*  Ejemplo: 
```bash
./server 7777
```
3. El servidor ya estará listo para recibir jugadores en ese puerto.
4. El cliente tendrá que escribir start para conectar con el servidor.
5. El servidor mostrará si existe un Juego nuevo. esto es debido dado a que un cliente escribió start en la terminal.
6. El servidor mostrará los movimientos de cada cliente y quién ganó.

## **Ejecutando el cliente:**

1. Abre una nueva terminal.
2. Navega hasta el directorio `src` donde están los archivos del juego. 
3. Para jugar contra el servidor, ejecuta el comando `./client <IP> <puerto>`, reemplazando `<IP>` por la dirección IP del servidor y `<puerto>` por el puerto que configuraste anteriormente. (IP puede ser la del localhost)
*  Ejemplo: 
```bash
./client 127.0.0.1 7777
```
4. Una vez realizada la conexión, debes escribir start en la terminal para comenzar una nueva partida.
5. El jugador que comienza el juego lo dicta el servidor. El primer turno lo podrás comenzar tú o el servidor
6. Para jugar debes introducir las fichas las cuales tienen como nombre "C". 
7. Una vez lanzada la ficha el servidor responderá y se refrescará el tablero con la representación de la ficha "S", la cual lanzó el servidor.
8. La partida terminará en victoria, derrota o empate.

