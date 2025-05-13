#ifndef DECK_H
#define DECK_H

#include <stack>
#include <vector>
#include "Card.h"

class Deck {
private:
    std::stack<Card*> drawPile;       // Stack for drawing cards
    std::stack<Card*> discardPile;    // Stack for discarded cards

    // Helper function to create and add the cards to the deck
    void addStandardUNODeck();

public:
    Deck();
    ~Deck();

    // Initialize the deck with 108 standard UNO cards
    void initializeDeck();

    // Shuffle the draw pile
    void shuffleDeck();

    // Draw a card from the deck (i.e., pop from drawPile)
    Card* drawCard();

    // Check if the draw pile is empty
    bool isEmpty() const;

    // Place a card in the discard pile
    void placeInDiscard(Card* card);

    // Move all cards from the discard pile back to the draw pile and shuffle
    void reshuffleDiscardIntoDeck();

    // Return a card to the draw pile
    void returnCardToDeck(Card* card);
    
    // Get the top card of the discard pile
    Card* getTopDiscard() const;
};

#endif // DECK_H