#ifndef ACTIONCARD_H
#define ACTIONCARD_H

#include "Card.h"

class ActionCard : public Card {
public:
    ActionCard(const std::string& cardName, Color cardColor = Color::NONE);
    virtual void play(UnoGame& game, Player* currentPlayer) = 0;
    virtual void print() const = 0;
    virtual std::string getName() const = 0;
    virtual ~ActionCard();
};

#endif // ACTIONCARD_H