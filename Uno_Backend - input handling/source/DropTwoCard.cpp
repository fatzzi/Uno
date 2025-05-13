// DropTwoCard.cpp
#include "../header/DropTwoCard.h"
#include "../header/UnoGame.h"
#include "../header/Player.h"
#include <iostream>
#include <algorithm>

DropTwoCard::DropTwoCard(Color color)
    : ActionCard("DropTwo", color) {}

void DropTwoCard::play(UnoGame& game, Player* currentPlayer) {
    std::cout << "Drop Two Card played! " << currentPlayer->getName() << " can drop up to 2 cards of their choice." << std::endl;

    int handSize = currentPlayer->getHandSize();
    if (handSize <= 0) {
        std::cout << "No cards to drop!" << std::endl;
        return;
    }

    currentPlayer->displayHand();

    int cardsToDrop = std::min(2, handSize);
    std::vector<int> selectedIndices;

    for (int i = 0; i < cardsToDrop; ++i) {
        int cardIndex;
        std::cout << "Select card " << (i + 1) << " to drop (0-" << (handSize - 1) << "), or -1 to stop dropping: ";

        while (true) {
            if (!game.getIntegerInput(cardIndex)) {
                std::cout << "Invalid input! Please enter a number between -1 and " << (handSize - 1) << ": ";
                continue;
            }

            if (cardIndex == -1) {
                break;  // user chose to stop dropping
            }

            if (cardIndex < 0 || cardIndex >= handSize) {
                std::cout << "Invalid card index! Enter a number between 0 and " << (handSize - 1) << ": ";
                continue;
            }

            // Check for duplicate selection
            if (std::find(selectedIndices.begin(), selectedIndices.end(), cardIndex) != selectedIndices.end()) {
                std::cout << "Card already selected! Choose a different one: ";
                continue;
            }

            selectedIndices.push_back(cardIndex);
            std::cout << "Selected to drop: ";
            currentPlayer->getCardAtIndex(cardIndex)->print();
            std::cout << std::endl;
            break;  // exit the input loop
        }

        if (cardIndex == -1) break;  // stop input early
    }

    // Sort indices in descending order to avoid shifting issues when removing
    std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

    for (int index : selectedIndices) {
        Card* cardToDrop = currentPlayer->getCardAtIndex(index);
        std::cout << "Dropping: ";
        cardToDrop->print();
        std::cout << std::endl;

        game.dropCardFromPlayer(currentPlayer, index);
    }

    std::cout << currentPlayer->getName() << " dropped " << selectedIndices.size() << " card(s)." << std::endl;
}

void DropTwoCard::print() const {
    std::cout << colorToString(color) << " Drop Two";
}

std::string DropTwoCard::getName() const {
    return "DropTwo";
}