#ifndef FOURMODEL_H
#define FOURMODEL_H

#include <vector>

class ConnectFour {
private:
    std::vector<std::vector<char>> board;
    static const int rows = 6;
    static const int cols = 7;
    char currentPlayer;

public:
    // Ejecución del juego
    ConnectFour();
    // Imprimir en pantalla el tablero
    void printBoard() const;
    // Colocar la ficha en la columna
    bool placeToken(int col);
    // Verificar si hay un ganador
    bool checkWin() const;
    // Cambiar de jugador
    void togglePlayer();
    // Obtener el jugador actual
    char getCurrentPlayer() const;
    // Obtener el número de filas
    static int getRows() { return rows; }
    // Obtener el número de columnas
    static int getCols() { return cols; }
    // Obtener el estado del tablero
    const std::vector<std::vector<char>>& getBoard() const { return board; }
};

#endif // FOURMODEL_H
