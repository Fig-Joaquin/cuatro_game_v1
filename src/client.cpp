#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);  // Cambia el puerto si es necesario

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return -1;
    }

    string mode;
    while (true) {
        cout << "Select mode: (1) Play against CPU, (2) Play against another player: ";
        cin >> mode;
        if (mode == "1" || mode == "2") {
            send(sock, mode.c_str(), mode.length(), 0);
            break;
        } else {
            cout << "Invalid selection. Please choose 1 or 2." << endl;
        }
    }

    // Recibir quién comienza el juego y mostrar el tablero inicial
    int valread = read(sock, buffer, 1024);
    buffer[valread] = '\0';
    cout << buffer << endl;

    string message;
    while (true) {
        valread = read(sock, buffer, 1024);
        buffer[valread] = '\0';
        cout << buffer << endl;
        if (strstr(buffer, "Your turn")) {
            cout << "Enter column (1-7) or Q to quit: ";
            cin >> message;
            if (message == "Q") {
                send(sock, message.c_str(), message.length(), 0);
                break;
            }
            if (message.length() == 1 && message[0] >= '1' && message[0] <= '7') {
                send(sock, message.c_str(), message.length(), 0);
            } else {
                cout << "Invalid input. Please enter a column number (1-7) or Q to quit." << endl;
                continue;
            }
        } else if (strstr(buffer, "win") || strstr(buffer, "draw") || strstr(buffer, "Opponent quit the game.")) {
            break;
        }
    }

    close(sock);
    return 0;
}
