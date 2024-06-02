# Juego Conecta Cuatro
Software desarrollado por Joaquín Sanchez e Ignacio Muñoz.

## Introducción

El juego consiste en una recreación del juego Conecta Cuatro. En pocas palabras, dos jugadores deben tirar fichas, 21 fichas como máximo por persona, si conectan 4 fichas de forma horizontal, vertical u horizontal se dicta el ganador del juego.

La recreación del juego, se desarrolló en C++ a través de sockets con la finalidad de que exista un cliente y un servidor. El cliente se conecta a través del servidor mediante la IP y un Puerto para crear una conexión y así el cliente pueda jugar contra el servidor. El cliente tiene por defecto la ficha de nombre C y el servidor tiene por defecto la ficha de nombre S.

## Aspectos técnicos del juego

**1. Cliente-Servidor:** Enfréntate contra el servidor en una partida multijugador a través de una conexión con el servidor.

* El juego se desarrolló en C++
+ Se utilizaron sockets para la comunicación entre el cliente y el servidor sobre el protocolo TCP.
* La lógica del juego está encapsulada en fourmodel.h y fourmodel.cpp.
* game.cpp contiene el bucle principal del juego para una versión local.
* Se implementó multithreading utilizando <thread> para manejar múltiples clientes concurrentemente en el servidor.
* Cada cliente se maneja en un hilo separado.


## Descripción detallada de los archivos

**SRC:**

* **client.cpp:** Controla la lógica del cliente, desde la conexión al servidor hasta la recepción y envío de mensajes.
* **fourmodel.cpp:** Define la lógica del juego en sí, incluyendo la representación del tablero, la colocación de fichas, la verificación de victorias y el cambio de turno.
* **fourmodel.h:** Sirve como cabecera para `fourmodel.cpp`, declarando las clases y funciones que forman el modelo del juego.
* **game.cpp:** Implementa la lógica principal del juego, inicializando la partida, gestionando los turnos, mostrando el tablero y detectando al ganador.
* **Makefile:** Contiene las instrucciones para compilar el proyecto de forma eficiente, simplificando el proceso de desarrollo.
* **server.cpp:** Toma las riendas del servidor, creando el socket de escucha, aceptando conexiones de clientes, gestionando hilos individuales para cada jugador y manejando el envío y recepción de mensajes.

## Requisitos previos para la compilación y ejecución
1. Tener instalado el gcc, g++ o por defecto el lenguaje C y C++.
2. Sistema operativo Linux. (No testeado en otros sistemas)

## Compilación y ejecución

Para preparar el juego para su ejecución, sigue estos sencillos pasos:

1. Abre una terminal y clona el repositorio. Puedes bajar el zip también. (Opcional)
2. Navega hasta la carpeta src mediante la terminal con los comandos cd.
3. Ejecuta el comando `make` para compilar el proyecto (Debes estar dentro la carpeta src). Esto generará los ejecutables `client` y `server` en la carpeta `src`.

## **Ejecutando el servidor:**

1. Abre una terminal y navega hasta el directorio `src` si es qué ya no estás en la carpeta.
2. Ejecuta el comando `./server <puerto>`, reemplazando `<puerto>` por el puerto que deseas utilizar para las conexiones. 
*  Ejemplo: 
```bash
./server 7777
```
3. El servidor ya estará listo para recibir jugadores en ese puerto.
4. El cliente tendrá que escribir start para conectar con el servidor.
5.  

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

