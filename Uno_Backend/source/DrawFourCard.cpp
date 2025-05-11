#include "../header/DrawFourCard.h"
#include"../header/Colour.h"
#include "../header/UnoGame.h"
#include <iostream>
using namespace std;
DrawFourCard::DrawFourCard() 
    : Card("DrawFour", Color::NONE) {}

void DrawFourCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Draw Four Card played! Choose a color (0-Red, 1-Yellow, 2-Green, 3-Blue): ";
    int choice;
    std::cin >> choice;
    
    switch(choice) {
        case 0: color = Color::RED; break;
        case 1: color = Color::YELLOW; break;
        case 2: color = Color::GREEN; break;
        case 3: color = Color::BLUE; break;
        default: color = Color::RED; break;  // Default to red if invalid input
    }
    
    std::cout << "Color changed to " << colorToString(color) << "!\n";
    game.makeNextPlayerDraw(4);
}

void DrawFourCard::print() const {
    std::cout << "Wild Draw Four";
    if (color != Color::NONE) {
        std::cout << " (" << colorToString(color) << ")";
    }
}

std::string DrawFourCard::getName() const {
    return "DrawFour";
}