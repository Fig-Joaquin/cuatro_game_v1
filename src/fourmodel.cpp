// fourmodel.cpp
#include "fourmodel.h"
#include <iostream>

ConnectFour::ConnectFour() : board(rows, std::vector<char>(cols, ' ')), currentPlayer('X') {}

void ConnectFour::printBoard() const {
    for (int row = 0; row < rows; row++) {
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

bool ConnectFour::checkWin() const {
    char token = currentPlayer;
    // Horizontal check
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col <= cols - 4; col++) {
            if (board[row][col] == token && board[row][col + 1] == token &&
                board[row][col + 2] == token && board[row][col + 3] == token) {
                return true;
            }
        }
    }

    // Vertical check
    for (int col = 0; col < cols; col++) {
        for (int row = 0; row <= rows - 4; row++) {
            if (board[row][col] == token && board[row + 1][col] == token &&
                board[row + 2][col] == token && board[row + 3][col] == token) {
                return true;
            }
        }
    }

    // Diagonal check (down-right)
    for (int row = 0; row <= rows - 4; row++) {
        for (int col = 0; col <= cols - 4; col++) {
            if (board[row][col] == token && board[row + 1][col + 1] == token &&
                board[row + 2][col + 2] == token && board[row + 3][col + 3] == token) {
                return true;
            }
        }
    }

    // Diagonal check (up-right)
    for (int row = 3; row < rows; row++) {
        for (int col = 0; col <= cols - 4; col++) {
            if (board[row][col] == token && board[row - 1][col + 1] == token &&
                board[row - 2][col + 2] == token && board[row - 3][col + 3] == token) {
                return true;
            }
        }
    }

    return false;
}

void ConnectFour::togglePlayer() {
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

char ConnectFour::getCurrentPlayer() const {
    return currentPlayer;
}
