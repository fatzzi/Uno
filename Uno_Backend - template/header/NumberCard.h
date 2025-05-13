#ifndef NUMBERCARD_H
#define NUMBERCARD_H

#include "Card.h"

class NumberCard : public Card {
private:
    int number;

public:
    NumberCard(int num, Color col);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
    Color getColor() const override;
    int getNumber() const;
};

#endif // NUMBERCARD_H