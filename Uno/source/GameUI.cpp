#include "../header/GameUI.h"
#include <cstring>
#include<algorithm>
using namespace std;
bool GameUI::effectsApplied = false;

GameUI::GameUI() 
    : awaitingPlayerChange(true), // Indicates the game starts with the player transition screen
      exitRequested(false),// Whether the user has requested to return to the main menu
      viewingEndScreen(false),// Whether the end game screen is being displayed
      cardDrawnThisTurn(false),// Whether the current player has drawn a card this turn
      showContinueButton(false),// Whether the "End Turn" button should be shown
      cardNeedingColorChoice(nullptr), 
      selectingCardsToDrop(false){
       // Tracks if a card requires a color to be chosen (Wild/DrawFour)
    strcpy(statusMessage, ""); // Clear status message at initialization
    selectedCardIndices.clear();
    // Initialize fullscreen toggle button
    fullscreenToggleButton.rect = {(float)(GetScreenWidth() - 180), 10, 160, 40}; // Button positioned at top-right corner (10px from top, 180px from right edge)
    fullscreenToggleButton.text = "Toggle Fullscreen";
    fullscreenToggleButton.color = (Color){65, 105, 225, 255}; // Royal blue
    fullscreenToggleButton.hoverColor = (Color){100, 149, 237, 255}; // Cornflower blue
    fullscreenToggleButton.isHovered = false;
}

bool GameUI::IsButtonClicked(Button& button, Vector2 mousePos) {
    // Checks if the mouse is inside the button's rectangle
    button.isHovered = CheckCollisionPointRec(mousePos, button.rect);
    // If mouse is inside and left button was pressed, return true
    return button.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
// Renders a button with hover effects and centered text
void GameUI::DrawButton(const Button& button) {
     // Choose color based on hover state
    DrawRectangleRec(button.rect, button.isHovered ? button.hoverColor : button.color);
     // Choose color based on hover state
    DrawText(button.text, 
             (int)(button.rect.x + button.rect.width/2 - MeasureText(button.text, 24)/2), 
             (int)(button.rect.y + button.rect.height/2 - 12), // Approximate vertical center
             24, BLACK);
}
// Sets the status message shown to the player
void GameUI::SetStatusMessage(const char* message) {
    strcpy(statusMessage, message);
}

// Returns whether the game UI has requested to exit to main menu
bool GameUI::IsExiting() const {
    return exitRequested;
}
// Draws all cards in the player's hand and stores their screen rectangles for click detection
void GameUI::DrawPlayerHand(Player* player, std::vector<Rectangle>& cardRects) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    int cardWidth = 60;
    int cardHeight = 90;
    int cardSpacing = 10;
    int maxCardsPerRow = 12; // Maximum cards to show in a row
    
    // Calculate rows needed
    int handSize = player->getHandSize();
    int rows = (handSize + maxCardsPerRow - 1) / maxCardsPerRow; // Ceiling division
    // Determine how many cards will be in the last row
    int cardsInLastRow = handSize % maxCardsPerRow;
    if (cardsInLastRow == 0 && handSize > 0) cardsInLastRow = maxCardsPerRow;
    
    // Create clickable area for each card
    cardRects.clear();  // Clear previous frame's rectangles
    
    // Display cards row by row
    DrawText("Your Cards:", 40, screenHeight - (rows * (cardHeight + 40)) - 40, 30, WHITE);
    
    for (int row = 0; row < rows; row++) {
        // For each row, calculate how many cards to draw
        int cardsInThisRow = (row == rows - 1) ? cardsInLastRow : maxCardsPerRow;
   // Total width of all cards + spacing in this row
        int totalRowWidth = (cardWidth + cardSpacing) * cardsInThisRow - cardSpacing;
        // Center row horizontally on screen
        int startX = screenWidth/2 - totalRowWidth/2;
        // Start drawing vertically from the bottom of the screen upwards        
        int startY = screenHeight - (rows - row) * (cardHeight + 40);
        
        for (int col = 0; col < cardsInThisRow; col++) {
            int cardIndex = row * maxCardsPerRow + col;
            int x = startX + col * (cardWidth + cardSpacing);
            Rectangle cardRect = {(float)x, (float)startY, (float)cardWidth, (float)cardHeight};
            cardRects.push_back(cardRect);
            
            // Draw card
            player->getCardAtIndex(cardIndex)->DrawCard(x, startY);
            
            // Draw card index for click
            DrawText(TextFormat("%d", cardIndex), x + cardWidth/2 - 5, startY + cardHeight + 5, 20, WHITE);
        }
    }
}
// Draws the current top card in the center of the screen
void GameUI::DrawTopCard(Card* topCard) {
    int screenWidth = GetScreenWidth();
    
    if (topCard) {
        // Draw "Top Card" label centered
        DrawText("Top Card:", screenWidth/2 - MeasureText("Top Card:", 30)/2, 80, 30, WHITE);
        // Draw card graphic centered horizontally
        topCard->DrawCard(screenWidth/2 - 30, 120); // Card is 60px wide, so offset by 30px to center
    }
}
// Displays which player's turn it currently is
void GameUI::DrawPlayerInfo(Player* player) {
    int screenWidth = GetScreenWidth();
    
    // Draw player's name centered at the top
    DrawText(TextFormat("%s's Turn", player->getName().c_str()), 
             screenWidth/2 - MeasureText(TextFormat("%s's Turn", player->getName().c_str()), 40)/2, 
             20, 40, WHITE);
}
// Draws all main interaction buttons and the fullscreen toggle
void GameUI::DrawGameButtons(Button& unoButton, Button& drawButton, Button& quitButton) {
    // Position buttons at top-left, center-top, and top-right
    int screenHeight = GetScreenHeight();
    int screenWidth = GetScreenWidth();

    quitButton.rect.y = 60;  // Top left
    quitButton.rect.x = 40;
    
    drawButton.rect.y = 60;  // Top middle
    drawButton.rect.x = screenWidth/2 - 90.0f; // Centered around midpoint
    
    unoButton.rect.y = 60;   // Top right
    unoButton.rect.x = screenWidth - 220.0f;
    
    // Apply button colors
    unoButton.color = (Color){255, 105, 180, 255}; // Hot pink
    unoButton.hoverColor = (Color){255, 182, 193, 255}; // Light pink
    
    drawButton.color = (Color){60, 179, 113, 255}; // Medium sea green
    drawButton.hoverColor = (Color){144, 238, 144, 255}; // Light green
    
    quitButton.color = (Color){255, 69, 0, 255}; // Orange red
    quitButton.hoverColor = (Color){255, 140, 0, 255}; // Dark orange
    
    DrawButton(unoButton);
    DrawButton(drawButton);
    DrawButton(quitButton);
    
    // Draw fullscreen toggle button
    UpdateFullscreenButton();
    DrawButton(fullscreenToggleButton);
}

void GameUI::DrawEndGameScreen(UnoGame& game) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int buttonWidth = 200;
    int buttonHeight = 60;
    
    DrawText("Game Over!", screenWidth/2 - MeasureText("Game Over!", 40)/2, screenHeight/2 - 100, 40, YELLOW);
    
    // Find winner if there is one
    int originalIndex = 0; // Remember starting player
    for (int i = 0; i < game.getPlayerCount(); i++) {
        Player* p = game.getCurrentPlayer();
        if (p && p->hasWon()) {
            char winText[64];
            sprintf(winText, "%s wins!", p->getName().c_str());
            DrawText(winText, screenWidth/2 - MeasureText(winText, 30)/2, screenHeight/2 - 40, 30, WHITE);
            break;
        }
        game.nextTurn(); // Check next player
        
        // If we've gone through all players, exit the loop
        if (i == game.getPlayerCount() - 1) {
            // Reset to original player index
            for (int j = 0; j < originalIndex; j++) {
                game.nextTurn();
            }
        }
    }
    
    // Draw main menu button
    Rectangle menuButton = {(float)(screenWidth/2 - buttonWidth/2), (float)(screenHeight/2 + 40), (float)buttonWidth, (float)buttonHeight};
    bool menuHover = CheckCollisionPointRec(GetMousePosition(), menuButton);
    
    DrawRectangleRec(menuButton, menuHover ? (Color){255, 140, 0, 255} : (Color){255, 69, 0, 255});
    DrawText("Main Menu", screenWidth/2 - MeasureText("Main Menu", 30)/2, (int)(menuButton.y + buttonHeight/2 - 15), 30, BLACK);
    
    if (menuHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        exitRequested = true; // Signal to return to main menu
    }
    
    // Draw fullscreen toggle button
    UpdateFullscreenButton();
    DrawButton(fullscreenToggleButton);
}

void GameUI::DrawPlayerTransitionScreen(const std::string& nextPlayerName) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int buttonWidth = 200;
    int buttonHeight = 60;
    
    // Dark overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 200});
    
    // Message box
    DrawRectangle(screenWidth/2 - 250, screenHeight/2 - 100, 500, 200, (Color){0, 0, 128, 255}); // Dark blue
    DrawRectangleLines(screenWidth/2 - 250, screenHeight/2 - 100, 500, 200, (Color){65, 105, 225, 255}); // Royal blue
    
    char message[64];
    sprintf(message, "Pass device to %s", nextPlayerName.c_str());
    DrawText(message, 
             screenWidth/2 - MeasureText(message, 30)/2, 
             screenHeight/2 - 60, 30, WHITE);
    
    // Continue button with matching main menu style
    Rectangle continueButton = {(float)(screenWidth/2 - buttonWidth/2), (float)(screenHeight/2 + 20), (float)buttonWidth, (float)buttonHeight};
    bool continueHover = CheckCollisionPointRec(GetMousePosition(), continueButton);
    
    DrawRectangleRec(continueButton, continueHover ? (Color){144, 238, 144, 255} : (Color){60, 179, 113, 255});
    DrawText("I am ready", screenWidth/2 - MeasureText("I am ready", 30)/2, (int)(continueButton.y + buttonHeight/2 - 15), 30, BLACK);
    
    if (continueHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        awaitingPlayerChange = false;
        cardDrawnThisTurn = false; // Reset for new player's turn
        showContinueButton = false;
    }
    
    // Draw fullscreen toggle button
    UpdateFullscreenButton();
    DrawButton(fullscreenToggleButton);
}

void GameUI::RequestColorChoice(Card* card) {
    cardNeedingColorChoice = card;
    colorSelector.Show();
}

void GameUI::UpdateFullscreenButton() {
    // Update position based on current screen dimensions
    fullscreenToggleButton.rect.x = GetScreenWidth() - 180;
    fullscreenToggleButton.rect.y = 10;
    
    // Check for hover and click
    Vector2 mousePos = GetMousePosition();
    fullscreenToggleButton.isHovered = CheckCollisionPointRec(mousePos, fullscreenToggleButton.rect);
    
    if (fullscreenToggleButton.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ToggleFullscreen();
    }
}

bool GameUI::HandleGameScreen(UnoGame& game) {
    // Use darker green to match main menu aesthetic
    ClearBackground((Color){40, 0, 50, 255}); // Dark navy background
    
    // Get screen dimensions
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Get current player
    Player* currentPlayer = game.getCurrentPlayer();
    if (!currentPlayer) return false;
    
    // Check for game over first
    if (game.isGameOver()) {
        if (!viewingEndScreen) viewingEndScreen = true;
        DrawEndGameScreen(game);
        return true;  // Still updating
    }
    
    // Handle color selector if active
    if (colorSelector.IsActive()) {
        if (colorSelector.Update()) {
            // A color was selected
            if (cardNeedingColorChoice) {
                cardNeedingColorChoice->setColor(colorSelector.GetSelectedColor());
                strcpy(statusMessage, "Color selected!");
                
                showContinueButton = true;
                cardNeedingColorChoice = nullptr;
            }
        }
        return true;
    }
    
    // Handle player transition screen
    if (awaitingPlayerChange) {
        DrawPlayerTransitionScreen(currentPlayer->getName());
        return true;
    }
    
    // Apply any pending effects from the last turn's action cards
    // This needs to happen immediately after transition screen is dismissed
    // but before the current player sees their hand
    static bool effectsApplied = false;
    if (!effectsApplied) {
        game.applyPendingEffects();
        effectsApplied = true;
        
        // If a skip was applied, we need another transition screen
        if (game.hasPendingSkipEffect()) {
            awaitingPlayerChange = true;
            effectsApplied = false;  // Reset for next transition
            return true;
        }
    }
    
    // Draw player info and top card
    DrawPlayerInfo(currentPlayer);
    DrawTopCard(game.getTopCard());
    
    // Create buttons
    Button unoButton = {
        {screenWidth - 220.0f, screenHeight - 120.0f, 180.0f, 50.0f},
        "Call UNO!",
        (Color){255, 105, 180, 255}, // Hot pink
        (Color){255, 182, 193, 255}, // Light pink
        false
    };
    
    Button drawButton = {
        {screenWidth/2 - 90.0f, screenHeight - 120.0f, 180.0f, 50.0f},
        "Draw Card",
        (Color){60, 179, 113, 255}, // Medium sea green
        (Color){144, 238, 144, 255}, // Light green
        false
    };
    
    Button quitButton = {
        {40.0f, screenHeight - 120.0f, 180.0f, 50.0f},
        "Main Menu",  // Changed from "Quit Game" to make purpose clearer
        (Color){255, 69, 0, 255}, // Orange red
        (Color){255, 140, 0, 255}, // Dark orange
        false
    };
    
    // Continue button for after drawing a card
    Button continueButton = {
        {screenWidth/2 - 90.0f, 350.0f, 180.0f, 50.0f},
        "End Turn",
        (Color){60, 179, 113, 255}, // Medium sea green
        (Color){144, 238, 144, 255}, // Light green
        false
    };
    
    // Track card positions for click detection
    std::vector<Rectangle> cardRects;
    
    // Draw player's hand
    DrawPlayerHand(currentPlayer, cardRects);
    
    // Draw status message (moved to prevent overlap)
    DrawText(statusMessage, screenWidth/2 - MeasureText(statusMessage, 20)/2, 220, 20, YELLOW);
    
    // Draw game buttons
    DrawGameButtons(unoButton, drawButton, quitButton);
    
    // Draw continue button if a card was drawn or played
    if (showContinueButton) {
        DrawButton(continueButton);
    }
    
    // Handle user input
    Vector2 mousePos = GetMousePosition();
    
    // Check if buttons are clicked
    if (IsButtonClicked(unoButton, mousePos)) {
        currentPlayer->callUNO();
        strcpy(statusMessage, 
               currentPlayer->getHandSize() == 2 ? 
               "UNO called successfully!" : 
               "You must have exactly 2 cards to call UNO!");
    }
    else if (IsButtonClicked(drawButton, mousePos)) {
        game.drawCard(currentPlayer);
        strcpy(statusMessage, "Card drawn. Look at your new card and end your turn when ready.");
        showContinueButton = true;
        cardDrawnThisTurn = true;
    }
    else if (showContinueButton && IsButtonClicked(continueButton, mousePos)) {
        // Player has seen their drawn card and wants to end turn
        game.nextTurn();
        awaitingPlayerChange = true;
        showContinueButton = false;
        cardDrawnThisTurn = false;
        strcpy(statusMessage, "");
        effectsApplied = false;  // Reset for next player
    }
    else if (IsButtonClicked(quitButton, mousePos)) {
        // Set exit flag to return to main menu
        exitRequested = true;
        strcpy(statusMessage, "Returning to main menu...");
        return true;
    }
    
    // Check for card clicks (only if continue button is not showing)
    if (!showContinueButton) {
        for (int i = 0; i < cardRects.size(); i++) {
            if (CheckCollisionPointRec(mousePos, cardRects[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Card* selectedCard = currentPlayer->getCardAtIndex(i);
                 // If already in DropTwo selection mode
                if (selectingCardsToDrop) {
                    // Allow selecting cards to drop
                    // Check if already selected
                    if (std::find(selectedCardIndices.begin(), selectedCardIndices.end(), i) == selectedCardIndices.end()) {
                        selectedCardIndices.push_back(i);
                        strcpy(statusMessage, TextFormat("Selected card %d to drop. Select %d more.", i, 2 - (int)selectedCardIndices.size()));
                    } else {
                        strcpy(statusMessage, "Card already selected! Choose another.");
                    }
                    // If selected 2 cards, drop them
                    if (selectedCardIndices.size() == 2) {
                        // Drop the selected cards using UnoGame's dropCardFromPlayer
                        // Drop in descending order to avoid index shifts
                        std::sort(selectedCardIndices.begin(), selectedCardIndices.end(), std::greater<int>());
                        for (int index : selectedCardIndices) {
                            game.dropCardFromPlayer(currentPlayer, index);
                        }
                        selectingCardsToDrop = false;
                        selectedCardIndices.clear();
                        strcpy(statusMessage, "Dropped 2 cards. Your turn is over.");
                        showContinueButton = true;
                    }
                    // Consume this click handling and not play any card
                    continue;
                }
                if (game.isCardPlayable(selectedCard)) {
                    // Check if it's a Wild or Draw Four card
                    if (selectedCard->getName() == "Wild" || selectedCard->getName() == "DrawFour") {
                        // Remove the card from hand and set as top card first
                        currentPlayer->playCard(i, game);
                        
                        // Then show color selector
                        RequestColorChoice(game.getTopCard());
                        strcpy(statusMessage, "Choose a color for your Wild card");
                        game.enforceUNOCall(currentPlayer);

                    } 
                      else if (selectedCard->getName() == "DropTwo") {
                    // Get a copy of the selected card
                    Card* dropTwoCard = selectedCard;
                    
                    // Remove the card from player's hand without calling play()
                    currentPlayer->removeCardFromHand(i);
                    
                    // Manually set it as the top card
                    game.setTopCard(dropTwoCard);
                    
                    // Check for UNO call
                    game.enforceUNOCall(currentPlayer);
                    
                    // Enable card selection mode for dropping cards
                    selectingCardsToDrop = true;
                    selectedCardIndices.clear();
                    strcpy(statusMessage, "Drop Two played! Select up to 2 cards to drop.");
                }
                    else {
                        // Play the card normally
                        currentPlayer->playCard(i, game);
                        
                        // Check for UNO call
                        game.enforceUNOCall(currentPlayer);
                        
                        // Check if player won
                        if (currentPlayer->hasWon()) {
                            strcpy(statusMessage, TextFormat("%s wins!", currentPlayer->getName().c_str()));
                            viewingEndScreen = true;
                            return true;
                        } else {
                            strcpy(statusMessage, "Card played. Your turn is over.");
                            showContinueButton = true;  // Show continue button after playing a card too
                        }
                    }
                } else {
                    strcpy(statusMessage, "This card cannot be played! Try another or draw.");
                }
            }
        }
    }
    
    return true;
}