#include "../header/Card.h"

Card::Card(const std::string& cardName, Color cardColor)
    : name(cardName), color(cardColor) {}

Card::~Card() {}

Color Card::getColor() const {
    return color;
}

void Card::setColor(Color newColor) {
    color = newColor;
}