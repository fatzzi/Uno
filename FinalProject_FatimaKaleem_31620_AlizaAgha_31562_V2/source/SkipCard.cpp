#include "../header/SkipCard.h"
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include <iostream>

SkipCard::SkipCard(CardColor color)
    : ActionCard("Skip", color) {}

void SkipCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Skipping the next player's turn." << std::endl;
    
    // Instead of immediately skipping, we'll tell the game
    // to apply the skip effect after the transition screen
    //game.skipTurn(); if we do this it exposes the next players cards.
    game.setApplySkipEffect(true);
}

void SkipCard::print() const {
    std::cout << cardColorToString(color) << " Skip";
}

std::string SkipCard::getName() const {
    return "Skip";
}

void SkipCard::DrawCard(int x, int y) const {
    Color rayColor = getRaylibColor(color);
    DrawRectangle(x, y, 60, 90, rayColor);
    DrawText("SKIP", x + 20, y + 35, 20, BLACK);
}