#ifndef GAME_UI_H
#define GAME_UI_H

#include <string>
#include <vector>
#include "raylib.h"
#include "UnoGame.h"
#include "Player.h"
#include "ColorSelector.h"

// Button structure
struct Button {
    Rectangle rect;
    const char* text;
    Color color;
    Color hoverColor;
    bool isHovered;
};

class GameUI {
private:
    bool awaitingPlayerChange;  // Whether the game is showing the player transition screen
    bool exitRequested;         // Whether the player has requested to exit to main menu
    bool viewingEndScreen;      // Whether the game over screen is being shown
    bool cardDrawnThisTurn;     // Whether the current player has drawn a card this turn
    bool showContinueButton;    // Whether to show the continue/end turn button
    char statusMessage[128];    // Message to display to the player
    Button fullscreenToggleButton; // Button to toggle fullscreen mode
    Card* cardNeedingColorChoice;  // Card that needs a color to be chosen (Wild cards)
    ColorSelector colorSelector;   // UI for selecting colors for Wild cards
    
    bool selectingCardsToDrop; // Indicates if the player is selecting cards to drop
    std::vector<int> selectedCardIndices; // Stores indices of selected cards

    bool effectsApplied; // tracks if pending effects have been applied

public:
    GameUI();
    
    bool IsButtonClicked(Button& button, Vector2 mousePos);
    void DrawButton(const Button& button);
    void SetStatusMessage(const char* message);
    bool IsExiting() const;
    
    void DrawPlayerHand(Player* player, std::vector<Rectangle>& cardRects);
    void DrawTopCard(Card* topCard);
    void DrawPlayerInfo(Player* player);
    void DrawGameButtons(Button& unoButton, Button& drawButton, Button& quitButton);
    void DrawEndGameScreen(UnoGame& game);
    void DrawPlayerTransitionScreen(const std::string& nextPlayerName);
    
    void RequestColorChoice(Card* card);
    void UpdateFullscreenButton();
    
    bool HandleGameScreen(UnoGame& game);
};



#endif // GAME_UI_H
