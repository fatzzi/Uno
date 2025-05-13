#include "../header/DrawTwoCard.h"
#include "../header/UnoGame.h"
#include <iostream>

DrawTwoCard::DrawTwoCard(Color color)
    : ActionCard("DrawTwo", color) {}

void DrawTwoCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Two Card played! Next player must draw 2 cards.\n";
    game.makeNextPlayerDraw(2);
}

void DrawTwoCard::print() const {
    std::cout << colorToString(color) << " Draw Two";
}

std::string DrawTwoCard::getName() const {
    return "DrawTwo";
}