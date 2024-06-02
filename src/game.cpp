// Incluye el archivo de cabecera para la clase ConnectFour y las funciones de entrada/salida.
#include "fourmodel.h"
#include <iostream>

// Permite usar los elementos del espacio de nombres std directamente (como cout y cin sin el prefijo std::).
using namespace std;

int main() {
    // Crea una instancia del juego.
    ConnectFour game;
    // Inicializa una variable para controlar si ya hay un ganador.
    bool gameWon = false;
    // Contador de turnos jugados.
    int turns = 0;
    // Calcula el número máximo de turnos posibles basándose en el tamaño del tablero.
    const int maxTurns = ConnectFour::getRows() * ConnectFour::getCols();

    // Bucle que continúa mientras no haya ganador y no se alcancen los turnos máximos.
    while (!gameWon && turns < maxTurns) {
        // Imprime el estado actual del tablero.
        game.printBoard();
        // Solicita al jugador actual que introduzca la columna donde desea colocar su ficha.
        cout << "Jugador " << game.getCurrentPlayer() << ", Ingresa un numero (1-" << ConnectFour::getCols() << "): ";
        int col;
        cin >> col;
        // Ajusta el índice de la columna para que coincida con la indexación del array (0-based).
        col--;

        // Intenta colocar la ficha en la columna seleccionada.
        if (!game.placeToken(col)) {
            // Si la columna está llena o el índice es inválido, informa al jugador y repite el bucle.
            cout << "La columna está llena. Vuelve a intentar." << endl;
            continue;
        }

        // Comprueba si el movimiento resulta en una victoria.
        if (game.checkWin()) {
            // Si hay un ganador, imprime el tablero y el mensaje de victoria.
            game.printBoard();
            cout << "Jugador " << game.getCurrentPlayer() << " Gana!" << endl;
            gameWon = true;
        } else {
            // Si no hay ganador, cambia de jugador.
            game.togglePlayer();
            // Incrementa el contador de turnos.
            turns++;
        }
    }

    // Si se alcanza el número máximo de turnos sin un ganador, declara un empate.
    if (!gameWon) {
        game.printBoard();
        cout << "Empate!." << endl;
    }

    // Termina el programa.
    return 0;
}
