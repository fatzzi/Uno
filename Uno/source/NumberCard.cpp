#include "../header/NumberCard.h"
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include"../header/ColorUtils.h"
#include <iostream>

NumberCard::NumberCard(int num, CardColor col)
    : Card("NumberCard", col), number(num) {
    name = cardColorToString(col) + " " + std::to_string(num);
}

void NumberCard::play(UnoGame& game, Player* currentPlayer) {
    // Logic for number cards - no special action, just played as is
    std::cout << "Played " << cardColorToString(color) << " " << number << " card." << std::endl;
}

void NumberCard::print() const {
    std::cout << cardColorToString(color) << " " << number;
}

int NumberCard::getNumber() const {
    return number;
}

CardColor NumberCard::getColor() const {
    return color;
}

std::string NumberCard::getName() const {
    return name;
}

void NumberCard::DrawCard(int x, int y) const {
    Color rayColor = getRaylibColor(color);
    DrawRectangle(x, y, 60, 90, rayColor);
    DrawText(std::to_string(number).c_str(), x + 20, y + 35, 20, BLACK);
}
