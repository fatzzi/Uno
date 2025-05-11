#include "../header/SkipCard.h"
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include <iostream>

SkipCard::SkipCard(Color color) 
    : ActionCard("Skip", color) {}

void SkipCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Skipping the next player's turn." << std::endl;
    game.skipTurn();  // Skip next player's turn in the game logic
}

void SkipCard::print() const {
    std::cout << colorToString(color) << " Skip";
}

std::string SkipCard::getName() const {
    return "Skip";
}