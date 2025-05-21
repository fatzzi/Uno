#ifndef DRAWSIXCARD_H
#define DRAWSIXCARD_H

#include "ActionCard.h"

class DrawSixCard : public ActionCard {
public:
    DrawSixCard(CardColor color);
    void play(UnoGame& game, Player* currentPlayer) override;
    void print() const override;
    std::string getName() const override;
        void DrawCard(int x, int y) const override;

};

#endif // DRAWSIXCARD_H