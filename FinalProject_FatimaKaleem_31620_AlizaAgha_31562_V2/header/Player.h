#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

// Forward declarations
class Card;
class Deck;
class UnoGame;

class Player {
private:
    std::string name;
    std::vector<Card*> hand;
    bool hasCalledUNO;

public:
    // Constructor that sets the player's name
    Player(const std::string& n);
    
    // Adds a card to the player's hand
    void drawCard(Deck& deck);
    
    // Plays a card from the player's hand
    void playCard(int index, UnoGame& game);
    
    // Checks if the player has no cards left
    bool hasWon() const;
    
    // Displays the player's current cards
    void displayHand() const;
    
    // Player declares they have one card left
    void callUNO();
    
    // Returns whether the player has called UNO
    bool hasCalledUNOStatus() const;
    
    // Resets the UNO call status
    void resetUNOCall();
    
    // Removes a card from hand at a given index
    void removeCardFromHand(int index);
    
    // Retrieves the card at a given index
    Card* getCardAtIndex(int index) const;
    
    // Returns the size of the hand
    int getHandSize() const;
    
    // Gets the player's name
    std::string getName() const;
    void drawHand(int startX, int startY) const;
    
    // Destructor to clean up cards in hand
    ~Player();
};

#endif // PLAYER_H