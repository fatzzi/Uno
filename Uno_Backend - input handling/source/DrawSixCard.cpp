#include "../header/DrawSixCard.h"
#include "../header/UnoGame.h"
#include <iostream>

DrawSixCard::DrawSixCard(Color color)
    : ActionCard("DrawSix", color) {}

void DrawSixCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Six Card played! Next player must draw 6 cards.\n";
    game.makeNextPlayerDraw(6);
}

void DrawSixCard::print() const {
    std::cout << colorToString(color) << " Draw Six";
}

std::string DrawSixCard::getName() const {
    return "DrawSix";
}