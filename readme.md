# ¡Conecta Cuatro!

## Introducción

¡Bienvenido a Conecta Cuatro, un clásico juego de estrategia implementado en dos modos emocionantes!

**1. Cliente-Servidor:** Enfréntate a otros jugadores en partidas multijugador a través de un servidor central.

**2. Contra CPU:** Pon a prueba tus habilidades contra un desafiante oponente controlado por la computadora.

El objetivo del juego es ser el primero en conectar cuatro fichas de tu color en horizontal, vertical o diagonal.

## Estructura del proyecto

El proyecto está organizado en directorios bien definidos para facilitar la navegación y el mantenimiento:

* **BUILD:** Almacena los archivos ejecutables compilados para una ejecución inmediata.
* **CLIENT:** Contiene el código fuente del cliente, responsable de la interacción con el servidor y la interfaz del jugador.
* **SERVER:** Alberga el código fuente del servidor, encargado de gestionar las conexiones de los clientes y el desarrollo de las partidas.
* **SRC:** El núcleo del juego reside aquí, con la lógica principal, la representación del tablero y la interacción con los jugadores.
* **Makefile:** Define las instrucciones para compilar el proyecto de manera eficiente.

## Descripción detallada de los archivos

**SRC:**

* **client.cpp:** Controla la lógica del cliente, desde la conexión al servidor hasta la recepción y envío de mensajes, asegurando una experiencia de juego fluida.
* **fourmodel.cpp:** Define la lógica del juego en sí, incluyendo la representación del tablero, la colocación de fichas, la verificación de victorias y el cambio de turno.
* **fourmodel.h:** Sirve como cabecera para `fourmodel.cpp`, declarando las clases y funciones que forman el modelo del juego.
* **game.cpp:** Implementa la lógica principal del juego, inicializando la partida, gestionando los turnos, mostrando el tablero y detectando al ganador.
* **Makefile:** Contiene las instrucciones para compilar el proyecto de forma eficiente, simplificando el proceso de desarrollo.

**SERVER:**

* **server.cpp:** Toma las riendas del servidor, creando el socket de escucha, aceptando conexiones de clientes, gestionando hilos individuales para cada jugador y manejando el envío y recepción de mensajes.

## Compilación y ejecución

Para preparar el juego para su ejecución, sigue estos sencillos pasos:

1. Abre una terminal y navega hasta la raíz del directorio del proyecto.
2. Ejecuta el comando `make` para compilar el proyecto. Esto generará los ejecutables `client` y `server` en la carpeta `BUILD`.

**Ejecutando el servidor:**

1. Abre una terminal y navega hasta el directorio `BUILD`.
2. Ejecuta el comando `./BUILD/server <puerto>`, reemplazando `<puerto>` por el puerto que deseas utilizar para las conexiones. El servidor estará listo para recibir jugadores en ese puerto.

**Ejecutando el cliente:**

1. Abre una terminal en una computadora diferente (o en una nueva ventana de terminal en la misma computadora si estás jugando contra la CPU).
2. Navega hasta el directorio `BUILD`.
3. Para jugar contra el servidor, ejecuta el comando `./BUILD/client <IP> <puerto>`, reemplazando `<IP>` por la dirección IP del servidor y `<puerto>` por el puerto que configuraste anteriormente.
4. Para jugar contra la CPU, omite la dirección IP y ejecuta simplemente `./BUILD/client <puerto>`, utilizando el mismo puerto que configuraste para el servidor.

**Modo Contra CPU:**

1. Inicia el servidor en una terminal con el comando mencionado anteriormente.
2. En otra terminal, ejecuta el cliente para enfrentarte a la computadora.
3. Sigue las instrucciones en pantalla para disfrutar de una emocionante partida contra la computadora local.

**Modo Cliente-Servidor:**

1. Inicia el servidor en una terminal como se describió anteriormente.
2. En otras terminales, ejecuta el cliente en cada dispositivo que desee participar, especificando la dirección IP del servidor y el puerto configurado.
3. Los jugadores se conectarán al servidor y podrán disfrutar de partidas multijugador.
