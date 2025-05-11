#include "../header/ReverseCard.h"
#include "../header/UnoGame.h"
#include <iostream>

ReverseCard::ReverseCard(Color color)
    : ActionCard("Reverse", color) {}

void ReverseCard::play(UnoGame& game, Player* currentPlayer) {
    // Check the number of players in the game
    if (game.getPlayerCount() == 2) {
        // If there are 2 players, skip the next player's turn
        std::cout << "Reversing the direction of play! (acting as a skip)" << std::endl;
        game.skipTurn(); // Call the skip function
    } else {
        // Normal reverse functionality
        std::cout << "Reversing the direction of play!" << std::endl;
        game.reverseDirection();
    }
}
void ReverseCard::print() const {
    std::cout << colorToString(color) << " Reverse";
}

std::string ReverseCard::getName() const {
    return "Reverse";
}