#include "../header/Card.h"
#include "../header/NumberCard.h"
#include "../header/WildCard.h"
#include "../header/DrawFourCard.h"


Card::Card(const std::string& cardName, CardColor cardColor)
    : name(cardName), color(cardColor) {}

Card::~Card() {}

CardColor Card::getColor() const {
    return color;
}

void Card::setColor(CardColor newColor) {
    color = newColor;
}

bool Card::operator==(const Card& other) const {
    // Wild and DrawFour are always playable
    if (dynamic_cast<const WildCard*>(this) || dynamic_cast<const DrawFourCard*>(this)) {
        return true;
    }

    // Color match
    if (this->getColor() == other.getColor()) {
        return true;
    }

    // Same action type
    if (this->getName() == other.getName()) {
        return true;
    }

    // Number match (only if both are NumberCards)
    const NumberCard* thisNum = dynamic_cast<const NumberCard*>(this);
    const NumberCard* otherNum = dynamic_cast<const NumberCard*>(&other);
    if (thisNum && otherNum && thisNum->getNumber() == otherNum->getNumber()) {
        return true;
    }

    return false;
}
