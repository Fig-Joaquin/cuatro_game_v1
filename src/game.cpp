// game.cpp
#include "fourmodel.h"
#include <iostream>
using namespace std; // This allows us to use std without specifying it each time for cout, cin, etc.

int main() {
    ConnectFour game;
    bool gameWon = false;
    int turns = 0;
    const int maxTurns = ConnectFour::getRows() * ConnectFour::getCols(); // Use getters

    while (!gameWon && turns < maxTurns) {
        game.printBoard();
        cout << "Player " << game.getCurrentPlayer() << ", enter column (1-" << ConnectFour::getCols() << "): ";
        int col;
        cin >> col;
        col--;

        if (!game.placeToken(col)) {
            cout << "Column is full or invalid. Try again." << endl;
            continue;
        }

        if (game.checkWin()) {
            game.printBoard();
            cout << "Player " << game.getCurrentPlayer() << " wins!" << endl;
            gameWon = true;
        } else {
            game.togglePlayer();
            turns++;
        }
    }

    if (!gameWon) {
        game.printBoard();
        cout << "The game is a draw." << endl;
    }

    return 0;
}
