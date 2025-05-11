#ifndef SKIPCARD_H
#define SKIPCARD_H

#include "ActionCard.h"

class SkipCard : public ActionCard {
public:
    SkipCard(Color color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

#endif // SKIPCARD_H
