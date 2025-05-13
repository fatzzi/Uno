#include "../header/Deck.h"
#include "../header/Card.h"
#include "../header/NumberCard.h"
#include "../header/ActionCard.h"
#include "../header/SkipCard.h"
#include "../header/ReverseCard.h"
#include "../header/DrawTwoCard.h"
#include "../header/WildCard.h"
#include "../header/DrawFourCard.h"
#include "../header/DrawSixCard.h"
#include "../header/DropTwoCard.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <random>

Deck::Deck() {
    std::srand(std::time(0)); // Initialize random number generator for shuffling
}

Deck::~Deck() {
    // Cleanup: Delete all cards in the drawPile and discardPile
    while (!drawPile.empty()) {
        delete drawPile.top();
        drawPile.pop();
    }

    while (!discardPile.empty()) {
        delete discardPile.top();
        discardPile.pop();
    }
}

void Deck::initializeDeck() {
    addStandardUNODeck();  // Fill the deck with the standard UNO cards
}

void Deck::addStandardUNODeck() {
    // Standard UNO deck has 108 cards:
    // - 19 Blue, 19 Green, 19 Red, and 19 Yellow cards (76 total)
    // - 8 Skip, 8 Reverse, and 8 Draw Two cards (24 total)
    // - 4 Wild cards and 4 Wild Draw Four cards (8 total)
    // Added: 8 Draw Six and 8 Drop Two cards (16 total) - making 124 cards in total

    std::vector<Color> colors = {Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW};

    for (const auto& color : colors) {
        // One 0 card per color
        drawPile.push(new NumberCard(0, color));
        
        // Two of each 1-9 cards per color
        for (int i = 1; i <= 9; ++i) {
            drawPile.push(new NumberCard(i, color));
            drawPile.push(new NumberCard(i, color));
        }
        
        // Two of each action card per color
        for (int i = 0; i < 2; ++i) {
            drawPile.push(new SkipCard(color));
            drawPile.push(new ReverseCard(color));
            drawPile.push(new DrawTwoCard(color));
            drawPile.push(new DrawSixCard(color));  // New card type
            drawPile.push(new DropTwoCard(color));  // New card type
        }
    }

    // Add wild cards
    for (int i = 0; i < 4; ++i) {
        drawPile.push(new WildCard());
        drawPile.push(new DrawFourCard());
    }
}

void Deck::shuffleDeck() {
    std::vector<Card*> allCards;

    // Transfer all cards from the drawPile to a temporary vector
    while (!drawPile.empty()) {
        allCards.push_back(drawPile.top());
        drawPile.pop();
    }

    // Shuffle using modern C++ random facilities
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allCards.begin(), allCards.end(), g);

    // Push the shuffled cards back into the drawPile
    for (auto& card : allCards) {
        drawPile.push(card);
    }
}

Card* Deck::drawCard() {
    if (drawPile.empty()) {
        if (discardPile.empty()) {
            return nullptr; // No cards left in the game
        }
        reshuffleDiscardIntoDeck();
    }
    
    Card* top = drawPile.top();
    drawPile.pop();
    return top;
}

bool Deck::isEmpty() const {
    return drawPile.empty();
}

void Deck::placeInDiscard(Card* card) {
    discardPile.push(card);
}

void Deck::reshuffleDiscardIntoDeck() {
    // Save the top card of the discard pile
    Card* topCard = nullptr;
    if (!discardPile.empty()) {
        topCard = discardPile.top();
        discardPile.pop();
    }
    
    // Move all cards from discard pile to draw pile
    while (!discardPile.empty()) {
        drawPile.push(discardPile.top());
        discardPile.pop();
    }
    
    // Shuffle the draw pile
    shuffleDeck();
    
    // Put the top card back on the discard pile
    if (topCard) {
        discardPile.push(topCard);
    }
}

void Deck::returnCardToDeck(Card* card) {
    drawPile.push(card);
}

Card* Deck::getTopDiscard() const {
    if (!discardPile.empty()) {
        return discardPile.top();
    }
    return nullptr;
}