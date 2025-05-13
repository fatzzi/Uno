
#include <iostream>
#include "../header/UnoGame.h" // Adjusted path
#include "../header/Player.h"  // Adjusted path
#include <string>
#include <limits> 

int main() {
    UnoGame game;

    int numPlayers = 0;

    // Ask number of players between 2 and 10
    do {
        std::cout << "Enter number of players (2 to 10): ";
        std::cin >> numPlayers;
        if(std::cin.fail() || numPlayers < 2 || numPlayers > 10) {
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
            std::cout << "Invalid number of players. Please enter between 2 and 10." << std::endl;
            numPlayers = 0;
        }
    } while(numPlayers == 0);
    // Clear the input buffer after reading numPlayers
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // Get player names and add to game
    for (int i = 0; i < numPlayers; ++i) {
        std::string name;

        std::cout << "Enter name for player " << (i + 1) << ": ";
        std::getline(std::cin, name);
        Player* p = new Player(name);
        game.addPlayer(p);
    }

    // Start the game and play until over
    std::cout << "\nStarting UNO game!\n" << std::endl;
    game.startGame();

    while (!game.isGameOver()) {
        game.playTurn();

        Player* currentPlayer = game.getCurrentPlayer();
        if (currentPlayer && currentPlayer->getHandSize() >= 21) {
            std::cout << currentPlayer->getName() << " has 21 or more cards and will be eliminated." << std::endl;
            game.eliminatePlayer(currentPlayer);
        }


    }

    std::cout << "\nGame has ended." << std::endl;

    return 0;
}

