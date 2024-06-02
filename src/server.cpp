#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include "fourmodel.h"

using namespace std;

// Estructura para almacenar la información del cliente
struct ClienteInfo {
    int socket_cliente;
    struct sockaddr_in direccionCliente;
};

// Función para enviar el estado del tablero al cliente
void enviarTablero(int socket_cliente, const ConnectFour &game) {
    stringstream ss; // Crear un stream para construir el mensaje
    ss << "TABLERO\n";
    for (int row = 0; row < ConnectFour::getRows(); row++) {
        ss << row + 1;
        for (int col = 0; col < ConnectFour::getCols(); col++) {
            ss << "| " << game.getBoard()[row][col];
        }
        ss << "|\n";
    }
    // Añadir números de columna en la parte inferior
    for (int col = 0; col < ConnectFour::getCols(); col++) {
        ss << "  " << col + 1;
    }
    ss << "\n";
    // Enviar el mensaje al cliente
    send(socket_cliente, ss.str().c_str(), ss.str().length(), 0);
}

// Función que maneja el juego entre el cliente y la CPU (servidor)
void *jugarContraCPU(void *arg) {
    ClienteInfo *clienteInfo = (ClienteInfo *)arg;
    int socket_cliente = clienteInfo->socket_cliente;
    struct sockaddr_in direccionCliente = clienteInfo->direccionCliente;
    delete clienteInfo; // Liberar la memoria asignada para ClienteInfo

    ConnectFour game; // Crear una nueva instancia del juego
    bool gameWon = false; // Indicador de si el juego ha sido ganado
    int turns = 0; // Contador de turnos
    const int maxTurns = ConnectFour::getRows() * ConnectFour::getCols(); // Número máximo de turnos

    char buffer[1024]; // Buffer para recibir mensajes del cliente
    memset(buffer, '\0', sizeof(char) * 1024); // Inicializar el buffer con ceros
    int n_bytes = 0; // Número de bytes recibidos

    char ip[INET_ADDRSTRLEN]; // Almacenar la dirección IP del cliente en formato legible
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN); // Convertir la dirección IP

    cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Nuevo jugador." << endl;

    // Seleccionar al azar quién comienza el juego
    srand(time(0)); // Inicializar la semilla del generador de números aleatorios
    bool isPlayerTurn = rand() % 2 == 0; // Decidir aleatoriamente quién empieza
    if (isPlayerTurn) {
        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Jugador inicia." << endl;
        send(socket_cliente, "Empiezas tu.\n", 12, 0); // Informar al cliente que comienza
    } else {
        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Servidor inicia." << endl;
        send(socket_cliente, "Servidor inicia.\n", 17, 0); // Informar al cliente que el servidor comienza
        game.togglePlayer(); // Cambia a jugador 'S' para que el servidor juegue primero
    }

    enviarTablero(socket_cliente, game); // Enviar el estado inicial del tablero

    while (!gameWon && turns < maxTurns) {
        if (isPlayerTurn) {
            send(socket_cliente, "Es tu turno.\n", 13, 0);
            n_bytes = recv(socket_cliente, buffer, 1024, 0);
            if (n_bytes <= 0) {
                // Si ocurre un error en la comunicación
                cout << "Error en la comunicación con el jugador." << endl;
                send(socket_cliente, "Error en la comunicación, juego terminado.\n", 43, 0);
                close(socket_cliente);
                pthread_exit(NULL);
                return NULL;
            }

            buffer[n_bytes] = '\0'; // Asegurar que el buffer sea una cadena de caracteres

            if (buffer[0] == 'Q') {
                // Si el cliente decide salir del juego
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Sale del juego." << endl;
                send(socket_cliente, "El oponente salió del juego.\n", 30, 0);
                close(socket_cliente);
                break;
            }

            // Convertir la entrada del cliente (columna) de char a int
            int col = buffer[0] - '1';  // Convertir char a int (asumiendo entrada '1'-'7')
            if (!game.placeToken(col)) {
                const char* msg = "La columna está llena o es inválida. Inténtalo de nuevo.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                continue; // Pedir al cliente que intente de nuevo
            }
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << col + 1 << "." << endl;
        } else {
            // El servidor selecciona una columna al azar
            int col;
            do {
                col = rand() % ConnectFour::getCols();
            } while (!game.placeToken(col)); // Reintentar si la columna está llena
            stringstream ss;
            ss << "Servidor eligió la columna " << (col + 1) << ".\n";
            send(socket_cliente, ss.str().c_str(), ss.str().length(), 0);
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Servidor juega columna " << col + 1 << "." << endl;
        }

        enviarTablero(socket_cliente, game); // Enviar el estado actualizado del tablero

        if (game.checkWin()) {
            // Si hay un ganador
            send(socket_cliente, isPlayerTurn ? "¡Has ganado!\n" : "¡La Servidor ha ganado!\n", 19, 0);
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: " << (isPlayerTurn ? "Cliente" : "Servidor") << " ha ganado." << endl;
            gameWon = true;
        } else {
            game.togglePlayer(); // Cambiar de jugador
            isPlayerTurn = !isPlayerTurn; // Alternar el turno
            turns++; // Incrementar el contador de turnos
        }
    }

    if (!gameWon) {
        // Si se llega al máximo de turnos sin ganador, es un empate
        send(socket_cliente, "El juego es un empate.\n", 22, 0);
    }

    close(socket_cliente); // Cerrar el socket del cliente
    pthread_exit(NULL); // Terminar el hilo
    return NULL;
}

// Función que maneja la conexión con el cliente
void *manejarCliente(void *arg) {
    ClienteInfo *clienteInfo = (ClienteInfo *)arg;
    int socket_cliente = clienteInfo->socket_cliente;

    char buffer[1024]; // Buffer para recibir mensajes del cliente
    memset(buffer, '\0', sizeof(char) * 1024); // Inicializar el buffer con ceros
    int n_bytes = recv(socket_cliente, buffer, 1024, 0); // Recibir mensaje del cliente

    if (n_bytes > 0 && strcmp(buffer, "start") == 0) {
        // Si el cliente envía "start", comenzar el juego
        pthread_t thread_id; // Crear un nuevo hilo para manejar el juego
        if (pthread_create(&thread_id, NULL, jugarContraCPU, (void *)clienteInfo) != 0) {
            // Si ocurre un error al crear el hilo
            cout << "Error creando el hilo\n";
            close(socket_cliente); // Cerrar el socket del cliente
            delete clienteInfo; // Liberar la memoria de ClienteInfo
        } else {
            pthread_detach(thread_id); // Desvincular el hilo para que se limpie automáticamente
        }
    } else {
        // Si el mensaje no es "start", cerrar la conexión
        close(socket_cliente);
        delete clienteInfo;
    }

    return NULL;
}

// Función principal
int main(int argc, char **argv) {
    if (argc != 2) {
        // Verificar si se pasó el número de argumentos correcto
        cout << "Uso: " << argv[0] << " <puerto>\n";
        exit(EXIT_FAILURE); // Terminar el programa si los argumentos son incorrectos
    }

    int port = atoi(argv[1]); // Convertir el argumento del puerto a entero
    int socket_server = 0; // Descriptor del socket del servidor
    struct sockaddr_in direccionServidor; // Estructura para la dirección del servidor

    cout << "Creando socket de escucha...\n";
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error al crear el socket de escucha\n";
        exit(EXIT_FAILURE); // Terminar el programa si ocurre un error
    }

    cout << "Configurando estructura de dirección del socket...\n";
    memset(&direccionServidor, 0, sizeof(direccionServidor)); // Inicializar la estructura de dirección con ceros
    direccionServidor.sin_family = AF_INET; // Establecer la familia de direcciones a AF_INET (IPv4)
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY); // Aceptar conexiones desde cualquier dirección IP
    direccionServidor.sin_port = htons(port); // Establecer el puerto del servidor

    cout << "Enlazando el socket...\n";
    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        // Enlazar el socket a la dirección y puerto especificados
        cout << "Error al llamar a bind()\n";
        exit(EXIT_FAILURE); // Terminar el programa si ocurre un error
    }

    cout << "Llamando a listen...\n";
    if (listen(socket_server, 1024) < 0) {
        // Poner el socket en modo escucha
        cout << "Error al llamar a listen()\n";
        exit(EXIT_FAILURE); // Terminar el programa si ocurre un error
    }

    socklen_t addr_size = sizeof(struct sockaddr_in); // Tamaño de la estructura de dirección del cliente

    cout << "Esperando conexiones...\n";

    while (true) {
        int socket_cliente;
        struct sockaddr_in direccionCliente;
        socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size); // Aceptar nuevas conexiones
        if (socket_cliente < 0) {
            // Si ocurre un error al aceptar una conexión
            cout << "Error al llamar a accept()" << endl;
            continue; // Continuar esperando conexiones
        }

        ClienteInfo *clienteInfo = new ClienteInfo; // Crear una nueva estructura para el cliente
        clienteInfo->socket_cliente = socket_cliente;
        clienteInfo->direccionCliente = direccionCliente;

        pthread_t thread_id; // Crear un nuevo hilo para manejar la conexión del cliente
        if (pthread_create(&thread_id, NULL, manejarCliente, (void *)clienteInfo) != 0) {
             // Si ocurre un error al crear el hilo
            cout << "Error creando el hilo\n";
            close(socket_cliente); // Cerrar el socket del cliente
            delete clienteInfo; // Liberar la memoria de ClienteInfo
        } else {
            pthread_detach(thread_id); // Desvincular el hilo para que se limpie automáticamente
        }
    }

    close(socket_server); // Cerrar el socket del servidor
    return 0;
}
