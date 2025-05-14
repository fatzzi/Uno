#ifndef UNO_GAME_H
#define UNO_GAME_H

#include <vector>
#include <string>
#include "../header/Deck.h"

class Player;
class Card;

class UnoGame {
private:
    std::vector<Player*> players;
    Deck deck;
    Card* topCard;
    int currentPlayerIndex;
    bool isReverse;
    int cardsDrawn;
    bool gameEnded = false;
    
    // New flags to track pending card effects
    bool pendingSkipEffect = false;
    bool pendingReverseEffect = false;
    
    

public:
    UnoGame();
    ~UnoGame();
    bool getIntegerInput(int& output);
    void addPlayer(Player* player);
    void startGame();
    void nextTurn();
    void playTurn();
    void drawCard(Player* player);
    void skipTurn();
    void reverseDirection();
    void makeNextPlayerDraw(int numCards);
    void makeCurrentPlayerDrop(int numCards);
    void dropCardFromPlayer(Player* player, int index);
    void eliminatePlayer(Player* player);
    void enforceUNOCall(Player* player);
    void setTopCard(Card* card);
    void run();
    
    bool isCardPlayable(Card* playedCard);
    bool isGameOver();
    
    // New methods to handle delayed effects
    void setApplySkipEffect(bool value) { pendingSkipEffect = value; }
    void setApplyReverseEffect(bool value) { pendingReverseEffect = value; }
    bool hasPendingSkipEffect() const { return pendingSkipEffect; }
    bool hasPendingReverseEffect() const { return pendingReverseEffect; }
    void applyPendingEffects(); // Method to apply any pending effects
    
    Player* getCurrentPlayer() const;
    Card* getTopCard() const;
    int getPlayerCount() const;
    int nextPlayerIndex() const;
};

#endif // UNO_GAME_H