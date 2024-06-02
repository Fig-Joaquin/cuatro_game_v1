#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <IP> <port>\n";
        return -1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return -1;
    }

    string message;
    while (true) {
        cout << "Escribe 'start' para comenzar a jugar.\n#: ";
        cin >> message;
        if (message == "start") {
            send(sock, message.c_str(), message.length(), 0);
            break;
        }
        if (message == "Q") {
            close(sock);
            return 0;
        } else {
            cout << "Para jugar debes escribir: 'start'\nSi quieres terminar la sesión escribe: 'Q'\n#: ";
        }
    }

    // Recibir quién comienza el juego, quién es el jugador, y mostrar el tablero inicial
    int valread = read(sock, buffer, 1024);
    buffer[valread] = '\0';
    cout << buffer << endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer)); // Limpiar buffer
        valread = read(sock, buffer, 1024);
        if (valread <= 0) {
            cout << "Connection closed." << endl;
            break;
        }
        buffer[valread] = '\0';

        // Mostrar el mensaje recibido
        cout << buffer << endl;

        if (strstr(buffer, "Es tu turno")) {
            cout << "Escoge el número de columna (1-7) o Q para salir.\n#: ";
            cin >> message;
            if (message == "Q") {
                send(sock, message.c_str(), message.length(), 0);
                break;
            }
            if (message.length() == 1 && message[0] >= '1' && message[0] <= '7') {
                send(sock, message.c_str(), message.length(), 0);
            } else {
                cout << "Por favor. Escoge el número de columna (1-7) o Q para salir." << endl;
                continue;
            }
        } else if (strstr(buffer, "Ganaste!") || strstr(buffer, "El servidor ganó!") || strstr(buffer, "Juego termina en empate") || strstr(buffer, "Se perdió la conexión.")) {
            // Mostrar el mensaje final y salir del bucle
            cout << buffer << endl;
            break;
        }
    }

    close(sock);
    return 0;
}
