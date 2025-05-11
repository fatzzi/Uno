#ifndef WILDCARD_H
#define WILDCARD_H

#include "Card.h"

class WildCard : public Card {
public:
    WildCard();
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

#endif // WILDCARD_H