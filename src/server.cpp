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

struct ClienteInfo {
    int socket_cliente;
    struct sockaddr_in direccionCliente;
};

void enviarTablero(int socket_cliente, const ConnectFour &game) {
    stringstream ss;
    ss << "TABLERO\n";
    for (int row = 0; row < ConnectFour::getRows(); row++) {
        ss << row + 1;
        for (int col = 0; col < ConnectFour::getCols(); col++) {
            ss << "| " << game.getBoard()[row][col];
        }
        ss << "|\n";
    }
    for (int col = 0; col < ConnectFour::getCols(); col++) {
        ss << "  " << col + 1;
    }
    ss << "\n";

    send(socket_cliente, ss.str().c_str(), ss.str().length(), 0);
}

void *jugarContraCPU(void *arg) {
    ClienteInfo *clienteInfo = (ClienteInfo *)arg;
    int socket_cliente = clienteInfo->socket_cliente;
    struct sockaddr_in direccionCliente = clienteInfo->direccionCliente;
    delete clienteInfo; // Liberar la memoria asignada para ClienteInfo

    ConnectFour game;
    bool gameWon = false;
    int turns = 0;
    const int maxTurns = ConnectFour::getRows() * ConnectFour::getCols();

    char buffer[1024];
    memset(buffer, '\0', sizeof(char) * 1024);
    int n_bytes = 0;

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);

    cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Nuevo jugador." << endl;

    // Seleccionar al azar quién comienza el juego
    srand(time(0));
    bool isPlayerTurn = rand() % 2 == 0;
    if (isPlayerTurn) {
        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Jugador inicia." << endl;
        send(socket_cliente, "Empiezas tu.\n", 12, 0);
    } else {
        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Servidor inicia." << endl;
        send(socket_cliente, "Servidor inicia.\n", 17, 0);
        game.togglePlayer(); // Cambia a jugador 'S' para que el servidor juegue primero
    }

    enviarTablero(socket_cliente, game);

    while (!gameWon && turns < maxTurns) {
        if (isPlayerTurn) {
            send(socket_cliente, "Es tu turno.\n", 13, 0);
            n_bytes = recv(socket_cliente, buffer, 1024, 0);
            if (n_bytes <= 0) {
                cout << "Error en la comunicación con el jugador." << endl;
                send(socket_cliente, "Error en la comunicación, juego terminado.\n", 43, 0);
                close(socket_cliente);
                pthread_exit(NULL);
                return NULL;
            }

            buffer[n_bytes] = '\0';

            if (buffer[0] == 'Q') {
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Sale del juego." << endl;
                send(socket_cliente, "El oponente salió del juego.\n", 30, 0);
                close(socket_cliente);
                break;
            }

            int col = buffer[0] - '1';  // Convertir char a int (asumiendo entrada '1'-'7')
            if (!game.placeToken(col)) {
                const char* msg = "La columna está llena o es inválida. Inténtalo de nuevo.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                continue;
            }
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << col + 1 << "." << endl;
        } else {
            // El servidor selecciona una columna al azar
            int col;
            do {
                col = rand() % ConnectFour::getCols();
            } while (!game.placeToken(col));
            stringstream ss;
            ss << "Servidor eligió la columna " << (col + 1) << ".\n";
            send(socket_cliente, ss.str().c_str(), ss.str().length(), 0);
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Servidor juega columna " << col + 1 << "." << endl;
        }

        enviarTablero(socket_cliente, game);

        if (game.checkWin()) {
            send(socket_cliente, isPlayerTurn ? "¡Has ganado!\n" : "¡La Servidor ha ganado!\n", 19, 0);
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: " << (isPlayerTurn ? "Cliente" : "Servidor") << " ha ganado." << endl;
            gameWon = true;
        } else {
            game.togglePlayer();
            isPlayerTurn = !isPlayerTurn;
            turns++;
        }
    }

    if (!gameWon) {
        send(socket_cliente, "El juego es un empate.\n", 22, 0);
    }

    close(socket_cliente);
    pthread_exit(NULL);
    return NULL;
}

void *manejarCliente(void *arg) {
    ClienteInfo *clienteInfo = (ClienteInfo *)arg;
    int socket_cliente = clienteInfo->socket_cliente;

    char buffer[1024];
    memset(buffer, '\0', sizeof(char) * 1024);
    int n_bytes = recv(socket_cliente, buffer, 1024, 0);

    if (n_bytes > 0 && strcmp(buffer, "start") == 0) {
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, jugarContraCPU, (void *)clienteInfo) != 0) {
            cout << "Error creando el hilo\n";
            close(socket_cliente);
            delete clienteInfo;
        } else {
            pthread_detach(thread_id);
        }
    } else {
        close(socket_cliente);
        delete clienteInfo;
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <puerto>\n";
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor;

    cout << "Creando socket de escucha...\n";
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error al crear el socket de escucha\n";
        exit(EXIT_FAILURE);
    }

    cout << "Configurando estructura de dirección del socket...\n";
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    cout << "Enlazando el socket...\n";
    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cout << "Error al llamar a bind()\n";
        exit(EXIT_FAILURE);
    }

    cout << "Llamando a listen...\n";
    if (listen(socket_server, 1024) < 0) {
        cout << "Error al llamar a listen()\n";
        exit(EXIT_FAILURE);
    }

    socklen_t addr_size = sizeof(struct sockaddr_in);

    cout << "Esperando conexiones...\n";

    while (true) {
        int socket_cliente;
        struct sockaddr_in direccionCliente;
        socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size);
        if (socket_cliente < 0) {
            cout << "Error al llamar a accept()" << endl;
            continue;
        }

        ClienteInfo *clienteInfo = new ClienteInfo;
        clienteInfo->socket_cliente = socket_cliente;
        clienteInfo->direccionCliente = direccionCliente;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, manejarCliente, (void *)clienteInfo) != 0) {
            cout << "Error creando el hilo\n";
            close(socket_cliente);
            delete clienteInfo;
        } else {
            pthread_detach(thread_id);
        }
    }

    close(socket_server);
    return 0;
}
