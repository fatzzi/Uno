#ifndef DRAWTWOCARD_H
#define DRAWTWOCARD_H

#include "ActionCard.h"

class DrawTwoCard : public ActionCard {
public:
    DrawTwoCard(Color color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

#endif // DRAWTWOCARD_H