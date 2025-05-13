// DropTwoCard.h
#ifndef DROPTWOCARD_H
#define DROPTWOCARD_H

#include "ActionCard.h"

class DropTwoCard : public ActionCard {
public:
    DropTwoCard(Color color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
};

#endif // DROPTWOCARD_H
