#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream>
#include "Color.h"

// Forward declarations
class UnoGame;
class Player;

class Card {
protected:
    std::string name;
    Color color;

public:
    Card(const std::string& cardName, Color cardColor = Color::NONE);
    virtual void play(UnoGame& game, Player* currentPlayer) = 0;
    virtual void print() const = 0;
    virtual std::string getName() const = 0;
    virtual Color getColor() const;
    virtual void setColor(Color newColor);
    virtual ~Card();
};

#endif // CARD_H