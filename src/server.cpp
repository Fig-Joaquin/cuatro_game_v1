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

struct JuegoInfo {
    int socket_cliente1;
    int socket_cliente2;
    struct sockaddr_in direccionCliente1;
    struct sockaddr_in direccionCliente2;
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

    cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Nuevo jugador contra CPU." << endl;

    // Seleccionar al azar quién comienza el juego
    srand(time(0));
    bool isPlayerTurn = rand() % 2 == 0;
    if (isPlayerTurn) {
        send(socket_cliente, "You start.\n", 11, 0);
    } else {
        send(socket_cliente, "Server starts.\n", 15, 0);
        game.togglePlayer(); // Cambia a jugador 'S' para que el servidor juegue primero
    }

    enviarTablero(socket_cliente, game);

    while (!gameWon && turns < maxTurns && (n_bytes = recv(socket_cliente, buffer, 1024, 0)) > 0) {
        buffer[n_bytes] = '\0';

        if (buffer[0] == 'Q') {
            cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Sale del juego." << endl;
            close(socket_cliente);
            break;
        }

        int col;
        if (isPlayerTurn) {
            col = buffer[0] - '1'; // Convertir char a int (asumiendo entrada '1'-'7')
            if (!game.placeToken(col)) {
                const char *msg = "Column is full or invalid. Try again.\n";
                send(socket_cliente, msg, strlen(msg), 0);
                continue;
            }
        } else {
            // El servidor selecciona una columna al azar
            do {
                col = rand() % ConnectFour::getCols();
            } while (!game.placeToken(col));
            stringstream ss;
            ss << "Server chose column " << (col + 1) << ".\n";
            send(socket_cliente, ss.str().c_str(), ss.str().length(), 0);
        }

        enviarTablero(socket_cliente, game);

        if (game.checkWin()) {
            send(socket_cliente, isPlayerTurn ? "You win!\n" : "Server wins!\n", 12, 0);
            gameWon = true;
        } else {
            game.togglePlayer();
            isPlayerTurn = !isPlayerTurn;
            turns++;
            send(socket_cliente, "Move accepted\n", 15, 0);
        }
    }

    if (!gameWon) {
        send(socket_cliente, "The game is a draw.\n", 20, 0);
    }

    close(socket_cliente);
    pthread_exit(NULL);
    return NULL;
}

void *jugarContraJugador(void *arg) {
    JuegoInfo *juegoInfo = (JuegoInfo *)arg;
    int socket_cliente1 = juegoInfo->socket_cliente1;
    int socket_cliente2 = juegoInfo->socket_cliente2;
    struct sockaddr_in direccionCliente1 = juegoInfo->direccionCliente1;
    struct sockaddr_in direccionCliente2 = juegoInfo->direccionCliente2;
    delete juegoInfo; // Liberar la memoria asignada para JuegoInfo

    ConnectFour game;
    bool gameWon = false;
    int turns = 0;
    const int maxTurns = ConnectFour::getRows() * ConnectFour::getCols();

    char buffer[1024];
    memset(buffer, '\0', sizeof(char) * 1024);
    int n_bytes = 0;

    char ip1[INET_ADDRSTRLEN];
    char ip2[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente1.sin_addr), ip1, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(direccionCliente2.sin_addr), ip2, INET_ADDRSTRLEN);

    cout << "[" << ip1 << ":" << ntohs(direccionCliente1.sin_port) << "] vs [" << ip2 << ":" << ntohs(direccionCliente2.sin_port) << "] Juego iniciado." << endl;

    // Seleccionar al azar quién comienza el juego
    srand(time(0));
    bool isPlayer1Turn = rand() % 2 == 0;
    send(socket_cliente1, isPlayer1Turn ? "You start.\n" : "Opponent starts.\n", 20, 0);
    send(socket_cliente2, isPlayer1Turn ? "Opponent starts.\n" : "You start.\n", 20, 0);

    enviarTablero(socket_cliente1, game);
    enviarTablero(socket_cliente2, game);

    while (!gameWon && turns < maxTurns) {
        int currentSocket = isPlayer1Turn ? socket_cliente1 : socket_cliente2;
        int waitingSocket = isPlayer1Turn ? socket_cliente2 : socket_cliente1;

        // Informar al jugador actual que es su turno
        send(currentSocket, "Your turn\n", 10, 0);
        send(waitingSocket, "Opponent's turn\n", 15, 0);

        n_bytes = recv(currentSocket, buffer, 1024, 0);
        if (n_bytes <= 0) {
            cout << "Error en la comunicación con el jugador." << endl;
            send(socket_cliente1, "Error in communication, game terminated.\n", 40, 0);
            send(socket_cliente2, "Error in communication, game terminated.\n", 40, 0);
            close(socket_cliente1);
            close(socket_cliente2);
            pthread_exit(NULL);
            return NULL;
        }

        buffer[n_bytes] = '\0';

        if (buffer[0] == 'Q') {
            cout << "El jugador ha salido del juego." << endl;
            send(socket_cliente1, "Opponent quit the game.\n", 24, 0);
            send(socket_cliente2, "Opponent quit the game.\n", 24, 0);
            close(socket_cliente1);
            close(socket_cliente2);
            pthread_exit(NULL);
            return NULL;
        }

        int col = buffer[0] - '1'; // Convertir char a int (asumiendo entrada '1'-'7')
        if (!game.placeToken(col)) {
            const char *msg = "Column is full or invalid. Try again.\n";
            send(currentSocket, msg, strlen(msg), 0);
            continue;
        }

        enviarTablero(socket_cliente1, game);
        enviarTablero(socket_cliente2, game);

        if (game.checkWin()) {
            send(socket_cliente1, isPlayer1Turn ? "You win!\n" : "Opponent wins!\n", 15, 0);
            send(socket_cliente2, isPlayer1Turn ? "Opponent wins!\n" : "You win!\n", 15, 0);
            gameWon = true;
        } else {
            game.togglePlayer();
            isPlayer1Turn = !isPlayer1Turn;
            turns++;
        }
    }

    if (!gameWon) {
        send(socket_cliente1, "The game is a draw.\n", 20, 0);
        send(socket_cliente2, "The game is a draw.\n", 20, 0);
    }

    close(socket_cliente1);
    close(socket_cliente2);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <port>\n";
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    cout << "Creating listening socket ...\n";
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating listening socket\n";
        exit(EXIT_FAILURE);
    }

    cout << "Configuring socket address structure ...\n";
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    cout << "Binding socket ...\n";
    if (bind(socket_server, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor)) < 0) {
        cout << "Error calling bind()\n";
        exit(EXIT_FAILURE);
    }

    cout << "Calling listening ...\n";
    if (listen(socket_server, 1024) < 0) {
        cout << "Error calling listen()\n";
        exit(EXIT_FAILURE);
    }

    socklen_t addr_size = sizeof(struct sockaddr_in);

    cout << "Waiting client request ...\n";
    while (true) {
        int socket_cliente1;
        if ((socket_cliente1 = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cout << "Error calling accept()\n";
            continue;
        }

        // Recibir la elección de modo de juego del primer cliente
        char buffer[1024];
        int n_bytes = recv(socket_cliente1, buffer, 1024, 0);
        buffer[n_bytes] = '\0';
        string mode1(buffer);

        if (mode1 == "1") {
            ClienteInfo *cpuInfo = new ClienteInfo;
            cpuInfo->socket_cliente = socket_cliente1;
            cpuInfo->direccionCliente = direccionCliente;
            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, jugarContraCPU, (void *)cpuInfo) != 0) {
                cout << "Error creating thread\n";
                close(socket_cliente1);
                delete cpuInfo;
            } else {
                pthread_detach(thread_id); // Detach the thread so that it cleans up after itself
            }
        } else if (mode1 == "2") {
            // Esperar al segundo cliente y confirmar su modo de juego
            int socket_cliente2;
            struct sockaddr_in direccionCliente2;
            cout << "Esperando al segundo jugador..." << endl;

            if ((socket_cliente2 = accept(socket_server, (struct sockaddr *)&direccionCliente2, &addr_size)) < 0) {
                cout << "Error llamando a accept()" << endl;
                close(socket_cliente1);
                continue;
            }

            // Recibir la elección de modo de juego del segundo cliente
            n_bytes = recv(socket_cliente2, buffer, 1024, 0);
            buffer[n_bytes] = '\0';
            string mode2(buffer);

            if (mode2 == "2") {
                JuegoInfo *juegoInfo = new JuegoInfo;
                juegoInfo->socket_cliente1 = socket_cliente1;
                juegoInfo->socket_cliente2 = socket_cliente2;
                juegoInfo->direccionCliente1 = direccionCliente;
                juegoInfo->direccionCliente2 = direccionCliente2;

                pthread_t thread_id;
                if (pthread_create(&thread_id, NULL, jugarContraJugador, (void *)juegoInfo) != 0) {
                    cout << "Error creating thread\n";
                    close(socket_cliente1);
                    close(socket_cliente2);
                    delete juegoInfo;
                } else {
                    pthread_detach(thread_id); // Detach the thread so that it cleans up after itself
                }
            } else {
                cout << "Invalid game mode selected by second client.\n";
                close(socket_cliente1);
                close(socket_cliente2);
            }
        } else {
            cout << "Invalid game mode selected.\n";
            close(socket_cliente1);
        }
    }

    close(socket_server);
    return 0;
}
