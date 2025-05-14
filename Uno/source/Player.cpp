#include "../header/Player.h"
#include "../header/Card.h"
#include "../header/Deck.h"
#include "../header/UnoGame.h"
#include <iostream>

Player::Player(const std::string& n)
    : name(n), hasCalledUNO(false) {
}

void Player::drawCard(Deck& deck) {
    Card* drawnCard = deck.drawCard();
    if (drawnCard) {
        hand.push_back(drawnCard); // Add the drawn card to the player's hand
        // Reset UNO call if player has more than one card now
        if (hand.size() > 1) {
            resetUNOCall();
        }
    }
}

void Player::playCard(int index, UnoGame& game) {
    if (index >= 0 && index < hand.size()) {
        Card* cardToPlay = hand[index];
        
        // Remove the card from the player's hand
        hand.erase(hand.begin() + index);
        
        // Then play the card which applies its effects
        cardToPlay->play(game, this);
        
        // Update the top card in the game
        game.setTopCard(cardToPlay);
    }
}

bool Player::hasWon() const {
    return hand.empty(); // A player wins when they have no cards left
}

void Player::displayHand() const {
    std::cout << name << "'s hand:" << std::endl;
    for (int i = 0; i < hand.size(); ++i) {
        std::cout << i << ": ";
        hand[i]->print();
        std::cout << std::endl;
    }
}

void Player::callUNO() {
    if (hand.size() == 2) {
        hasCalledUNO = true;
        std::cout << name << " has called UNO!" << std::endl;
    } else {
        std::cout << "You must have 2 cards to call UNO!" << std::endl;
    }
}

bool Player::hasCalledUNOStatus() const {
    return hasCalledUNO;
}

void Player::resetUNOCall() {
    hasCalledUNO = false;
}

void Player::removeCardFromHand(int index) {
    if (index >= 0 && index < hand.size()) {
        // Don't delete the card as it will be used elsewhere (transferred ownership)
        hand.erase(hand.begin() + index);
    }
}

Card* Player::getCardAtIndex(int index) const {
    if (index >= 0 && index < hand.size()) {
        return hand[index];
    }
    return nullptr; // Return nullptr if the index is invalid
}

int Player::getHandSize() const {
    return hand.size();
}

std::string Player::getName() const {
    return name;
}
void Player::drawHand(int startX, int startY) const {
    const int cardWidth = 60;
    const int cardSpacing = 10;  // Space between cards
    
    for (int i = 0; i < hand.size(); ++i) {
        int x = startX + i * (cardWidth + cardSpacing);
        hand[i]->DrawCard(x, startY);
    }
}


Player::~Player() {
    for (Card* card : hand) {
        delete card; // Free the memory allocated for each card
    }
}