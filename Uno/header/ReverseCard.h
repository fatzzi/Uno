#ifndef REVERSECARD_H
#define REVERSECARD_H

#include "ActionCard.h"

class ReverseCard : public ActionCard {
public:
    ReverseCard(CardColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
        void DrawCard(int x, int y) const override;

};

#endif // REVERSECARD_H