#include "../header/NumberCard.h"
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include <iostream>

NumberCard::NumberCard(int num, Color col)
    : Card("NumberCard", col), number(num) {
    name = colorToString(col) + " " + std::to_string(num);
}

void NumberCard::play(UnoGame& game, Player* currentPlayer) {
    // Logic for number cards - no special action, just played as is
    std::cout << "Played " << colorToString(color) << " " << number << " card." << std::endl;
}

void NumberCard::print() const {
    std::cout << colorToString(color) << " " << number;
}

int NumberCard::getNumber() const {
    return number;
}

Color NumberCard::getColor() const {
    return color;
}

std::string NumberCard::getName() const {
    return name;
}