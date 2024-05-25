#include <sys/socket.h> // socket()
#include <arpa/inet.h>  // hton*()
#include <string.h>     // memset()
#include <unistd.h> 
#include <iostream>
#include <sstream>      // stringstream
#include <pthread.h> // pthread_create, pthread_detach
#include "fourmodel.h"

using namespace std;

struct ClienteInfo {
    int socket_cliente;
    struct sockaddr_in direccionCliente;
};

void *jugar(void *arg) {
    ClienteInfo *clienteInfo = (ClienteInfo *)arg;
    int socket_cliente = clienteInfo->socket_cliente;
    struct sockaddr_in direccionCliente = clienteInfo->direccionCliente;
    delete clienteInfo; // Liberar la memoria asignada para ClienteInfo

    ConnectFour game;
    bool gameWon = false;
    int turns = 0;
    const int maxTurns = ConnectFour::getRows() * ConnectFour::getCols();

    char buffer[1024];
    memset(buffer, '\0', sizeof(char)*1024);
    int n_bytes = 0;

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);

    cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Nuevo jugador." << endl;

    while (!gameWon && turns < maxTurns && (n_bytes = recv(socket_cliente, buffer, 1024, 0)) > 0) {
        buffer[n_bytes] = '\0';

        if (buffer[0] == 'Q') {
            cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Sale del juego." << endl;
            close(socket_cliente);
            break;
        }

        int col = buffer[0] - '1';  // Convertir char a int (asumiendo entrada '1'-'7')

        if (!game.placeToken(col)) {
            const char* msg = "Column is full or invalid. Try again.\n";
            send(socket_cliente, msg, strlen(msg), 0);
            continue;
        }

        // Enviar estado actual del tablero al cliente
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

        if (game.checkWin()) {
            send(socket_cliente, "You win!\n", 9, 0);
            gameWon = true;
        } else {
            game.togglePlayer();
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
    direccionServidor.sin_family      = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port        = htons(port);

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
        int socket_cliente;
        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cout << "Error calling accept()\n";
            exit(EXIT_FAILURE);
        }

        ClienteInfo *clienteInfo = new ClienteInfo;
        clienteInfo->socket_cliente = socket_cliente;
        clienteInfo->direccionCliente = direccionCliente;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, jugar, (void *)clienteInfo) != 0) {
            cout << "Error creating thread\n";
            close(socket_cliente);
            delete clienteInfo;
            continue;
        }

        pthread_detach(thread_id); // Detach the thread so that it cleans up after itself
    }

    return 0;
}
