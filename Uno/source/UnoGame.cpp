#include "../header/UnoGame.h"
#include "../header/Player.h"
#include "../header/Card.h"
#include "../header/NumberCard.h"
#include "../header/ActionCard.h"
#include "../header/WildCard.h"
#include "../header/DrawFourCard.h"
#include "../header/CardUtils.h"
#include <iostream>
#include <algorithm>
#include <limits> 
#include <sstream>

UnoGame::UnoGame()
    : topCard(nullptr), currentPlayerIndex(0), isReverse(false), cardsDrawn(0) {
}

void UnoGame::addPlayer(Player* player) {
    players.push_back(player);
}

void UnoGame::startGame() {
    deck.initializeDeck();
    deck.shuffleDeck();
    
    for (auto& player : players) {
        for (int i = 0; i < 7; ++i) {  // Each player draws 7 cards at the start
            player->drawCard(deck);
        }
    }
    
    // Draw the first card to start the game
    topCard = deck.drawCard();
    
    // If the first card is a special card, handle it
    if (dynamic_cast<WildCard*>(topCard) || dynamic_cast<DrawFourCard*>(topCard)) {
        // If it's a wild card, set a random color
        topCard->setColor(static_cast<CardColor>(rand() % 4));
    }
    
    std::cout << "Game Started! Top card is: ";
    topCard->print();
    std::cout << std::endl;
}

void UnoGame::nextTurn() {
    if (isReverse) {
        currentPlayerIndex--;
        if (currentPlayerIndex < 0) {
            currentPlayerIndex = players.size() - 1;
        }
    } else {
        currentPlayerIndex++;
        if (currentPlayerIndex >= players.size()) {
            currentPlayerIndex = 0;
        }
    }
}

int UnoGame::nextPlayerIndex() const {
    int nextIdx = currentPlayerIndex;
    if (isReverse) {
        nextIdx--;
        if (nextIdx < 0) nextIdx = players.size() - 1;
    } else {
        nextIdx++;
        if (nextIdx >= players.size()) nextIdx = 0;
    }
    return nextIdx;
}

void UnoGame::playTurn() {
    Player* currentPlayer = players[currentPlayerIndex];

    if (currentPlayer->hasWon()) return;

    std::cout << "\n-- " << currentPlayer->getName() << "'s turn --" << std::endl;
    std::cout << "Top card: ";
    topCard->print();
    std::cout << std::endl;

    std::cout << "Has the device been passed to the next player? Press any key to continue..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    currentPlayer->displayHand();

    int choice;
    bool validChoice = false;

    while (!validChoice) {
        std::cout << "Choose a card to play (0-" << (currentPlayer->getHandSize() - 1) 
                  << "), enter -1 to draw, -2 to call UNO, or -9 to quit: ";

        if (!getIntegerInput(choice)) {
            std::cout << "Invalid input! Please enter a number." << std::endl;
            continue;
        }

        if (choice == -9) {
            std::cout << currentPlayer->getName() << " has ended the game." << std::endl;
            gameEnded = true;
            return;
        }

        if (choice == -2) {
            currentPlayer->callUNO();
            continue; // Let them choose again
        }

        if (choice == -1) {
            std::cout << currentPlayer->getName() << " draws a card." << std::endl;
            drawCard(currentPlayer);
            validChoice = true;
        } else if (choice >= 0 && choice < currentPlayer->getHandSize()) {
            Card* selectedCard = currentPlayer->getCardAtIndex(choice);

            if (isCardPlayable(selectedCard)) {
                currentPlayer->playCard(choice, *this);
                validChoice = true;
                enforceUNOCall(currentPlayer);

                if (currentPlayer->hasWon()) {
                    std::cout << "\n*** " << currentPlayer->getName() << " wins! ***" << std::endl;
                    gameEnded = true;
                    return;
                }
            } else {
                std::cout << "That card cannot be played on the current top card. Please select a valid card or draw." << std::endl;
            }
        } else {
            std::cout << "Invalid choice! Please select a valid card or draw." << std::endl;
        }
    }

    nextTurn();
}



void UnoGame::drawCard(Player* player) {
    player->drawCard(deck);
    std::cout << player->getName() << " drew a card." << std::endl;
    
    // Check if player now has 21 or more cards after drawing
    if (player->getHandSize() >= 21) {
        std::cout << player->getName() << " has 21 or more cards and is eliminated!" << std::endl;
        eliminatePlayer(player);
    }
}


void UnoGame::skipTurn() {
    std::cout << "Skipping " << players[nextPlayerIndex()]->getName() << "'s turn!" << std::endl;
    nextTurn();
}

void UnoGame::reverseDirection() {
    isReverse = !isReverse;
    std::cout << "Game direction reversed!" << std::endl;
}

void UnoGame::makeNextPlayerDraw(int numCards) {
    int nextPlayerIdx = nextPlayerIndex();
    
    Player* nextPlayer = players[nextPlayerIdx];
    std::cout << nextPlayer->getName() << " must draw " << numCards << " cards!" << std::endl;
    
    for (int i = 0; i < numCards; ++i) {
        nextPlayer->drawCard(deck);
    }
    
    // Check if player now has 21 or more cards after drawing multiple cards
    if (nextPlayer->getHandSize() >= 21) {
        std::cout << nextPlayer->getName() << " has 21 or more cards and is eliminated!" << std::endl;
        eliminatePlayer(nextPlayer);
    }
}



void UnoGame::dropCardFromPlayer(Player* player, int index) {
    if (index >= 0 && index < player->getHandSize()) {
        Card* cardToDrop = player->getCardAtIndex(index);
        player->removeCardFromHand(index);
        deck.placeInDiscard(cardToDrop);
    }
}

void UnoGame::eliminatePlayer(Player* player) {
    auto it = std::find(players.begin(), players.end(), player);
    if (it != players.end()) {
        std::cout << player->getName() << " has been eliminated from the game." << std::endl;
        players.erase(it);
        
        // Adjust currentPlayerIndex if needed
        if (currentPlayerIndex >= players.size()) {
            currentPlayerIndex = 0;
        }
    }
}

bool UnoGame::isGameOver() {
    if (gameEnded) return true;
    for (auto& player : players) {
        if (player->hasWon()) {
            return true;
        }
    }
    return players.size() <= 1; // Game is also over if only one player remains
}

void UnoGame::enforceUNOCall(Player* player) {
    if (player->getHandSize() == 1 && !player->hasCalledUNOStatus()) {
        std::cout << player->getName() << " forgot to call UNO! Drawing 2 penalty cards." << std::endl;
        // Make the player draw two cards as penalty
        for (int i = 0; i < 2; ++i) {
            player->drawCard(deck);
        }
    }
}

bool UnoGame::isCardPlayable(Card* playedCard) {
    return areCardsPlayable(playedCard, topCard);
}


void UnoGame::setTopCard(Card* card) {
    // Place the previous top card in the discard pile
    if (topCard) {
        deck.placeInDiscard(topCard);
    }
    
    // Set the new top card
    topCard = card;
}

Player* UnoGame::getCurrentPlayer() const {
    if (currentPlayerIndex >= 0 && currentPlayerIndex < players.size()) {
        return players[currentPlayerIndex];
    }
    return nullptr;
}

Card* UnoGame::getTopCard() const {
    return topCard;
}

void UnoGame::run() {
    std::cout << "=== UNO GAME STARTED ===" << std::endl;
    startGame();
    
    while (!isGameOver()) {
        playTurn();
    }
    
    // Find the winner
    Player* winner = nullptr;
    for (auto& player : players) {
        if (player->hasWon()) {
            winner = player;
            break;
        }
    }
    
    if (winner) {
        std::cout << "\n*** " << winner->getName() << " wins the game! ***" << std::endl;
    } else if (players.size() == 1) {
        std::cout << "\n*** " << players[0]->getName() << " is the last player standing and wins! ***" << std::endl;
    } else {
        std::cout << "\n*** Game over with no winner ***" << std::endl;
    }
}

int UnoGame::getPlayerCount() const {
    return players.size();
}

bool UnoGame::getIntegerInput(int& output) {
    std::string line;
    std::getline(std::cin, line);
    std::stringstream ss(line);
    return (ss >> output) && ss.eof();
}

void UnoGame::applyPendingEffects() {
    // Apply reverse effect if pending
    if (pendingReverseEffect) {
        reverseDirection();
        pendingReverseEffect = false;
    }
    
    // Apply skip effect if pending
    if (pendingSkipEffect) {
        skipTurn(); // This will call nextTurn() internally
        pendingSkipEffect = false;
    }
}


UnoGame::~UnoGame() {
    // Clean up the top card if it exists
    if (topCard) {
        delete topCard;
    }
     for (Player* p : players) {
        delete p;
    }
    
}
