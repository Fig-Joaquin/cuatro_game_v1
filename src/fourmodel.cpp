// fourmodel.cpp
#include "fourmodel.h"
#include <iostream>

/// Constructor
ConnectFour::ConnectFour() : board(rows, std::vector<char>(cols, ' ')), currentPlayer('C') {}

// Imprime el estado actual del tablero de juego en la consola.
void ConnectFour::printBoard() const {
    std::cout << "TABLERO" << std::endl;
    for (int row = 0; row < rows; row++) {
        std::cout << row + 1;
        for (int col = 0; col < cols; col++) {
            std::cout << "| " << board[row][col];
        }
        std::cout << "|\n";
    }
    for (int col = 0; col < cols; col++) {
        std::cout << "  " << col + 1;
    }
    std::cout << std::endl;
}
// Colocar la ficha en la columna.
bool ConnectFour::placeToken(int col) {
    if (col < 0 || col >= cols || board[0][col] != ' ') {
        return false;
    }
    for (int row = rows - 1; row >= 0; row--) {
        if (board[row][col] == ' ') {
            board[row][col] = currentPlayer;
            return true;
        }
    }
    return false;
}
// Verificar si hay un ganador.
bool ConnectFour::checkWin() const {
    // Almacenar el símbolo del jugador actual ('X' o 'O') en la variable 'token'.
    char token = currentPlayer;

    // Comprobar horizontalmente a lo largo de todas las filas.
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col <= cols - 4; col++) {
            // Verificar si hay cuatro fichas consecutivas del mismo tipo en la fila actual.
            if (board[row][col] == token && board[row][col + 1] == token &&
                board[row][col + 2] == token && board[row][col + 3] == token) {
                return true;  // Si se encuentran, retornar verdadero (hay ganador).
            }
        }
    }

    // Comprobar verticalmente a lo largo de todas las columnas.
    for (int col = 0; col < cols; col++) {
        for (int row = 0; row <= rows - 4; row++) {
            // Verificar si hay cuatro fichas consecutivas del mismo tipo en la columna actual.
            if (board[row][col] == token && board[row + 1][col] == token &&
                board[row + 2][col] == token && board[row + 3][col] == token) {
                return true;  // Si se encuentran, retornar verdadero (hay ganador).
            }
        }
    }

    // Comprobar diagonalmente hacia abajo y hacia la derecha.
    for (int row = 0; row <= rows - 4; row++) {
        for (int col = 0; col <= cols - 4; col++) {
            // Verificar si hay cuatro fichas consecutivas del mismo tipo en diagonal descendente.
            if (board[row][col] == token && board[row + 1][col + 1] == token &&
                board[row + 2][col + 2] == token && board[row + 3][col + 3] == token) {
                return true;  // Si se encuentran, retornar verdadero (hay ganador).
            }
        }
    }

    // Comprobar diagonalmente hacia arriba y hacia la derecha.
    for (int row = 3; row < rows; row++) {
        for (int col = 0; col <= cols - 4; col++) {
            // Verificar si hay cuatro fichas consecutivas del mismo tipo en diagonal ascendente.
            if (board[row][col] == token && board[row - 1][col + 1] == token &&
                board[row - 2][col + 2] == token && board[row - 3][col + 3] == token) {
                return true;  // Si se encuentran, retornar verdadero (hay ganador).
            }
        }
    }

    return false;  // Si ninguna de las condiciones anteriores se cumple, retornar falso (no hay ganador).
}

// Cambia el turno al otro jugador.

void ConnectFour::togglePlayer() {
    // Cambia el jugador actual.
    // Si el jugador actual es 'S', lo cambia a 'C', y viceversa.
    currentPlayer = (currentPlayer == 'S') ? 'C' : 'S';
}

// Retorna el símbolo del jugador actual ('S' o 'C').
char ConnectFour::getCurrentPlayer() const {
    // Devuelve el símbolo del jugador actual.
    return currentPlayer;
}
