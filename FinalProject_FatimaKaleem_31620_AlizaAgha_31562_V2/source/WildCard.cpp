#include "../header/WildCard.h"
#include "../header/UnoGame.h"
#include "../header/ColorUtils.h"
#include <iostream>

WildCard::WildCard() 
    : Card("Wild", CardColor::NONE) {}

void WildCard::play(UnoGame& game, Player* currentPlayer) {

    
    std::cout << "Color changed to " << cardColorToString(color) << "!\n";
}

void WildCard::print() const {
    std::cout << "Wild Card";
    if (color != CardColor::NONE) {
        std::cout << " (" << cardColorToString(color) << ")";
    }
}

std::string WildCard::getName() const {
    return "Wild";
}
void WildCard::DrawCard(int x, int y) const {
    DrawRectangle(x, y, 60, 90, DARKGRAY); // Background for wild
    DrawText("W", x + 25, y + 35, 20, WHITE);
    if (color != CardColor::NONE) {
    Color rayColor = getRaylibColor(color);
    DrawRectangle(x + 45, y + 5, 10, 10, rayColor); // Small color indicator
}

}
