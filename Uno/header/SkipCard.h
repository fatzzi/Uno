#ifndef SKIPCARD_H
#define SKIPCARD_H

#include "ActionCard.h"

class SkipCard : public ActionCard {
public:
    SkipCard(CardColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
        void DrawCard(int x, int y) const override;

};

#endif // SKIPCARD_H
