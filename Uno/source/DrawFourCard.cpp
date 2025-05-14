#include "../header/DrawFourCard.h"
#include"../header/CardColour.h"
#include "../header/UnoGame.h"
#include "../header/ColorUtils.h"
#include <iostream>
using namespace std;
DrawFourCard::DrawFourCard() 
    : Card("DrawFour", CardColor::NONE) {}

void DrawFourCard::play(UnoGame& game, Player* currentPlayer) {
    game.makeNextPlayerDraw(4);
}

void DrawFourCard::print() const {
    std::cout << "Wild Draw Four";
    if (color != CardColor::NONE) {
        std::cout << " (" << cardColorToString(color) << ")";
    }
}

std::string DrawFourCard::getName() const {
    return "DrawFour";
}

void DrawFourCard::DrawCard(int x, int y) const {
    DrawRectangle(x, y, 60, 90, DARKGRAY); // Background for draw four
    DrawText("D4", x + 20, y + 35, 20, WHITE);

    if (color != CardColor::NONE) {
        Color rayColor = getRaylibColor(color);
        DrawRectangle(x + 45, y + 5, 10, 10, rayColor); // Chosen color indicator
    }
}
// Draw the card with the chosen color
