#ifndef DRAWFOURCARD_H
#define DRAWFOURCARD_H

#include "Card.h"

class DrawFourCard : public Card {
public:
    DrawFourCard();
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
    void DrawCard(int x, int y) const override;

};

#endif // DRAWFOURCARD_H