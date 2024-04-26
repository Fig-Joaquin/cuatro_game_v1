// fourmodel.h
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
    ConnectFour();
    void printBoard() const;
    bool placeToken(int col);
    bool checkWin() const;
    void togglePlayer();
    char getCurrentPlayer() const;
    static int getRows() { return rows; }
    static int getCols() { return cols; }
};

#endif // FOURMODEL_H
