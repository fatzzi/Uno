#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream>
#include "CardColour.h"

// Forward declarations
class UnoGame;
class Player;

class Card {
protected:
    std::string name;
    CardColor color;

public:
    Card(const std::string& cardName, CardColor cardColor = CardColor::NONE);
    virtual void play(UnoGame& game, Player* currentPlayer) = 0;
    virtual void print() const = 0;
    virtual std::string getName() const = 0;
    virtual CardColor getColor() const;
    virtual void setColor(CardColor newColor);
    virtual bool operator==(const Card& other) const;
    virtual void DrawCard(int x, int y) const = 0;
    virtual ~Card();
};

#endif // CARD_H