#include "../header/ReverseCard.h"
#include "../header/UnoGame.h"
#include <iostream>

ReverseCard::ReverseCard(CardColor color)
    : ActionCard("Reverse", color) {}

void ReverseCard::play(UnoGame& game, Player* currentPlayer) {
    // Check the number of players in the game
    if (game.getPlayerCount() == 2) {
        // If there are 2 players, mark it for skipping the next player's turn
        std::cout << "Reversing the direction of play! (acting as a skip)" << std::endl;
        game.setApplySkipEffect(true);
    } else {
        // Normal reverse functionality, but delay until after transition
        std::cout << "Reversing the direction of play!" << std::endl;
        game.reverseDirection();
    }
}

void ReverseCard::print() const {
    std::cout << cardColorToString(color) << " Reverse";
}

std::string ReverseCard::getName() const {
    return "Reverse";
}

void ReverseCard::DrawCard(int x, int y) const {
    Color rayColor = getRaylibColor(color);
    DrawRectangle(x, y, 60, 90, rayColor);
    DrawText("RVS", x + 20, y + 35, 20, BLACK);
}