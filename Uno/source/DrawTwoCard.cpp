#include "../header/DrawTwoCard.h"
#include "../header/UnoGame.h"
#include <iostream>

DrawTwoCard::DrawTwoCard(CardColor color)
    : ActionCard("DrawTwo", color) {}

void DrawTwoCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Two Card played! Next player must draw 2 cards.\n";
    game.makeNextPlayerDraw(2);
}

void DrawTwoCard::print() const {
    std::cout << cardColorToString(color) << " Draw Two";
}

std::string DrawTwoCard::getName() const {
    return "DrawTwo";
}
void DrawTwoCard::DrawCard(int x, int y) const {
    Color rayColor = getRaylibColor(color);
    DrawRectangle(x, y, 60, 90, rayColor);
    DrawText("D2", x + 20, y + 35, 20, BLACK);
}
