#ifndef NUMBERCARD_H
#define NUMBERCARD_H

#include "Card.h"

class NumberCard : public Card {
private:
    int number;

public:
    NumberCard(int num, CardColor col);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
    CardColor getColor() const override;
    int getNumber() const;
    void DrawCard(int x, int y) const override;

};

#endif // NUMBERCARD_H