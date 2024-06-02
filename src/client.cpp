#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main(int argc, char **argv) {
    // Verificar argumentos de línea de comandos (IP y puerto)
    if (argc != 3) {
        cout << "Verificar: " << argv[0] << " <IP> <port>\n";
        return -1;
    }

    const char *server_ip = argv[1]; // Almacenar dirección IP del servidor
    int port = atoi(argv[2]); // Convertir puerto de string a entero

    // Variables para comunicación por socket
    int sock = 0; // Socket para conexión con el servidor
    struct sockaddr_in serv_addr; // Estructura de dirección del servidor
    char buffer[1024] = {0}; // Buffer para almacenar datos enviados y recibidos

    // Crear socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Falló en la creación del socket" << endl;
        return -1;
    }

    // Inicializar estructura de dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convertir dirección IP a dirección de red
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        cout << "IP invalida" << endl;
        return -1;
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Conexión fallida" << endl;
        return -1;
    }

    string message; // Variable para almacenar mensajes enviados y recibidos
    while (true) {
        // Solicitar al usuario que escriba "start" para comenzar
        cout << "Escribe 'start' para comenzar a jugar.\n#: ";
        cin >> message;
        // Verificar si el usuario quiere comenzar o salir
        if (message == "start") {
            send(sock, message.c_str(), message.length(), 0); // Enviar mensaje "start"
            break;
        }
        if (message == "Q") {
            close(sock);
            return 0;
        }
        else {
            // Informar al usuario sobre los comandos válidos
            cout << "Para jugar debes escribir: 'start'\nSi quieres terminar la sesión escribe: 'Q'\n#: ";
        }
    }

    // Recibir quién comienza el juego, quién es el jugador, y mostrar el tablero inicial
    int valread = read(sock, buffer, 1024);
    buffer[valread] = '\0';
    cout << buffer << endl; // Mostrar información recibida

    // Bucle de juego principal
    while (true) {
        // Limpiar buffer antes de recibir nuevos datos
        memset(buffer, 0, sizeof(buffer)); // Limpiar buffer
        valread = read(sock, buffer, 1024);
        // Verificar si la conexión se cerró
        if (valread <= 0) {
            cout << "Fin del juego." << endl;
            break;
        }
        buffer[valread] = '\0';

        // Mostrar el mensaje recibido del servidor
        cout << buffer << endl;

        // Verificar si es tu turno
        if (strstr(buffer, "Es tu turno")) {
            // Solicitar al usuario el número de columna o "Q" para salir
            while (true) {
                cout << "Escoge el número de columna (1-7) o Q para salir.\n#: ";
                cin >> message;
                // Verificar si el usuario quiere salir
                if (message == "Q") {
                    send(sock, message.c_str(), message.length(), 0);
                    close(sock);
                    return 0;
                }
                // Validar la elección de columna del usuario
                if (message.length() == 1 && message[0] >= '1' && message[0] <= '7') {
                    send(sock, message.c_str(), message.length(), 0);
                    break;
                } else {
                    cout << "Por favor. Escoge el número de columna (1-7) o Q para salir." << endl;
                }
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
