#include "../header/GameUI.h"
#include <cstring>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include "../header/Exceptions.h" // Include custom exception header

using namespace std;
bool GameUI::effectsApplied = false;

// Constructor for GameUI class
GameUI::GameUI()
    : awaitingPlayerChange(true), // Indicates the game starts with the player transition screen
      exitRequested(false),       // Whether the user has requested to return to the main menu
      viewingEndScreen(false),    // Whether the end game screen is being displayed
      cardDrawnThisTurn(false),   // Whether the current player has drawn a card this turn
      showContinueButton(false),  // Whether the "End Turn" button should be shown
      cardNeedingColorChoice(nullptr), // Tracks if a card requires a color to be chosen (Wild/DrawFour)
      selectingCardsToDrop(false) // Tracks if the player is selecting cards to drop (for DropTwo card)
{
    strcpy(statusMessage, ""); // Clear status message at initialization
    selectedCardIndices.clear(); // Clear selected card indices
    // Initialize fullscreen toggle button properties
    fullscreenToggleButton.rect = {(float)(GetScreenWidth() - 180), 10, 160, 40}; // Button positioned at top-right corner
    fullscreenToggleButton.text = "Fullscreen";
    fullscreenToggleButton.color = (Color){65, 105, 225, 255};       // Royal blue
    fullscreenToggleButton.hoverColor = (Color){100, 149, 237, 255}; // Cornflower blue
    fullscreenToggleButton.isHovered = false;
}

// Checks if a button is clicked based on mouse position and left mouse button press
bool GameUI::IsButtonClicked(Button &button, Vector2 mousePos)
{
    // Checks if the mouse is inside the button's rectangle
    button.isHovered = CheckCollisionPointRec(mousePos, button.rect);
    // If mouse is inside and left button was pressed, return true
    return button.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Renders a button with hover effects and centered text
void GameUI::DrawButton(const Button &button)
{
    // Choose color based on hover state
    DrawRectangleRec(button.rect, button.isHovered ? button.hoverColor : button.color);
    // Draw button text centered
    DrawText(button.text,
             (int)(button.rect.x + button.rect.width / 2 - MeasureText(button.text, 24) / 2),
             (int)(button.rect.y + button.rect.height / 2 - 12), // Approximate vertical center
             24, BLACK);
}

// Sets the status message shown to the player
void GameUI::SetStatusMessage(const char *message)
{
    // Copy the message to the statusMessage buffer, ensuring null-termination
    strncpy(statusMessage, message, sizeof(statusMessage) - 1);
    statusMessage[sizeof(statusMessage) - 1] = '\0'; // Ensure null-termination
}

// Returns whether the game UI has requested to exit to main menu
bool GameUI::IsExiting() const
{
    return exitRequested;
}

// Draws all cards in the player's hand and stores their screen rectangles for click detection
void GameUI::DrawPlayerHand(Player *player, std::vector<Rectangle> &cardRects)
{
    // Check for null player pointer to prevent crashes
    if (!player) {
        throw Uno::NullPointerException("player in DrawPlayerHand");
    }

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int cardWidth = 60;
    int cardHeight = 90;
    int cardSpacing = 10;
    int maxCardsPerRow = 12; // Maximum cards to show in a row

    int handSize = player->getHandSize();
    int rows = (handSize + maxCardsPerRow - 1) / maxCardsPerRow; // Calculate number of rows needed (ceiling division)
    int cardsInLastRow = handSize % maxCardsPerRow;
    if (cardsInLastRow == 0 && handSize > 0)
        cardsInLastRow = maxCardsPerRow; // If last row is full, it has maxCardsPerRow cards

    cardRects.clear(); // Clear previous frame's rectangles to avoid accumulation

    // Draw "Your Cards:" label
    DrawText("Your Cards:", 40, screenHeight - (rows * (cardHeight + 40)) - 40, 30, WHITE);

    // Iterate through rows and columns to draw each card
    for (int row = 0; row < rows; row++)
    {
        int cardsInThisRow = (row == rows - 1) ? cardsInLastRow : maxCardsPerRow; // Determine cards in current row
        int totalRowWidth = (cardWidth + cardSpacing) * cardsInThisRow - cardSpacing; // Calculate total width of cards in row
        int startX = screenWidth / 2 - totalRowWidth / 2; // Center row horizontally
        int startY = screenHeight - (rows - row) * (cardHeight + 40); // Position rows from bottom upwards

        for (int col = 0; col < cardsInThisRow; col++)
        {
            int cardIndex = row * maxCardsPerRow + col;
            int x = startX + col * (cardWidth + cardSpacing);
            Rectangle cardRect = {(float)x, (float)startY, (float)cardWidth, (float)cardHeight};
            cardRects.push_back(cardRect); // Store card rectangle for click detection

            try {
                Card* card = player->getCardAtIndex(cardIndex); // Get card from player's hand
                if (!card) {
                    throw Uno::CardException(TextFormat("Card at index %d is null in player's hand.", cardIndex));
                }
                card->DrawCard(x, startY); // Draw the card
            } catch (const Uno::CardException& e) {
                SetStatusMessage(TextFormat("Card Error: %s", e.what()));
                // Draw a placeholder or error indicator if card cannot be drawn
                DrawRectangle(x, startY, cardWidth, cardHeight, RED);
                DrawText("ERROR", x + 10, startY + 30, 20, WHITE);
            } catch (const std::exception& e) {
                SetStatusMessage(TextFormat("Unexpected Error: %s", e.what()));
            }
            
            // Draw card index for user reference
            DrawText(TextFormat("%d", cardIndex), x + cardWidth / 2 - 5, startY + cardHeight + 5, 20, WHITE);
        }
    }
}

// Draws the current top card in the center of the screen
void GameUI::DrawTopCard(Card *topCard)
{
    int screenWidth = GetScreenWidth();

    if (topCard)
    {
        // Draw "Top Card" label and the card graphic
        DrawText("Top Card:", screenWidth / 2 - MeasureText("Top Card:", 30) / 2, 80, 30, WHITE);
        topCard->DrawCard(screenWidth / 2 - 30, 120); // Card is 60px wide, so offset by 30px to center
    } else {
        // Handle case where topCard is null by displaying an error message and a placeholder
        SetStatusMessage("Error: Top card is not set.");
        DrawRectangle(screenWidth / 2 - 30, 120, 60, 90, GRAY); // Draw a placeholder rectangle
        DrawText("NO CARD", screenWidth / 2 - MeasureText("NO CARD", 15) / 2, 150, 15, BLACK); // Display "NO CARD"
    }
}

// Displays which player's turn it currently is
void GameUI::DrawPlayerInfo(Player *player)
{
    // Check for null player pointer to prevent crashes
    if (!player) {
        throw Uno::NullPointerException("player in DrawPlayerInfo");
    }

    int screenWidth = GetScreenWidth();

    // Draw player's name centered at the top
    DrawText(TextFormat("%s's Turn", player->getName().c_str()),
             screenWidth / 2 - MeasureText(TextFormat("%s's Turn", player->getName().c_str()), 40) / 2,
             20, 40, WHITE);
}

// Draws all main interaction buttons and the fullscreen toggle
void GameUI::DrawGameButtons(Button &unoButton, Button &drawButton, Button &quitButton)
{
    int screenHeight = GetScreenHeight();
    int screenWidth = GetScreenWidth();

    // Position buttons at top-left, center-top, and top-right
    quitButton.rect.y = 60; // Top left
    quitButton.rect.x = 40;

    drawButton.rect.y = 60;                      // Top middle
    drawButton.rect.x = screenWidth / 2 - 90.0f; // Centered around midpoint

    unoButton.rect.y = 60; // Top right
    unoButton.rect.x = screenWidth - 220.0f;

    // Apply button colors for visual distinction
    unoButton.color = (Color){255, 105, 180, 255};      // Hot pink
    unoButton.hoverColor = (Color){255, 182, 193, 255}; // Light pink

    drawButton.color = (Color){60, 179, 113, 255};       // Medium sea green
    drawButton.hoverColor = (Color){144, 238, 144, 255}; // Light green

    quitButton.color = (Color){255, 69, 0, 255};       // Orange red
    quitButton.hoverColor = (Color){255, 140, 0, 255}; // Dark orange

    // Draw the main game buttons
    DrawButton(unoButton);
    DrawButton(drawButton);
    DrawButton(quitButton);

    // Update and draw the fullscreen toggle button
    UpdateFullscreenButton();
    DrawButton(fullscreenToggleButton);
}

// Draws the end game screen, displaying the winner (if any) and a main menu button
void GameUI::DrawEndGameScreen(UnoGame &game)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int buttonWidth = 200;
    int buttonHeight = 60;

    DrawText("Game Over!", screenWidth / 2 - MeasureText("Game Over!", 40) / 2, screenHeight / 2 - 100, 40, YELLOW);

    // Find winner by iterating through players
    Player* winner = nullptr;
    try {
        // Iterate through players to find a winner
        for (int i = 0; i < game.getPlayerCount(); i++) {
            Player *p = game.getCurrentPlayer();
            if (p && p->hasWon()) {
                winner = p;
                break;
            }
            game.nextTurn(); // Move to the next player to check
        }
    } catch (const Uno::UnoException& e) {
        SetStatusMessage(TextFormat("Game Error: %s", e.what()));
    } catch (const std::exception& e) {
        SetStatusMessage(TextFormat("Unexpected Error: %s", e.what()));
    }

    if (winner) {
        char winText[64];
        sprintf(winText, "%s wins!", winner->getName().c_str());
        DrawText(winText, screenWidth / 2 - MeasureText(winText, 30) / 2, screenHeight / 2 - 40, 30, WHITE);
    } else {
        DrawText("No winner found.", screenWidth / 2 - MeasureText("No winner found.", 30) / 2, screenHeight / 2 - 40, 30, WHITE);
    }

    // Draw main menu button
    Rectangle menuButton = {(float)(screenWidth / 2 - buttonWidth / 2), (float)(screenHeight / 2 + 40), (float)buttonWidth, (float)buttonHeight};
    bool menuHover = CheckCollisionPointRec(GetMousePosition(), menuButton);

    DrawRectangleRec(menuButton, menuHover ? (Color){255, 140, 0, 255} : (Color){255, 69, 0, 255});
    DrawText("Main Menu", screenWidth / 2 - MeasureText("Main Menu", 30) / 2, (int)(menuButton.y + buttonHeight / 2 - 15), 30, BLACK);

    // If main menu button is clicked, set exitRequested flag
    if (menuHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        exitRequested = true; // Signal to return to main menu
    }

    UpdateFullscreenButton(); // Update and draw fullscreen toggle button
    DrawButton(fullscreenToggleButton);
}

// Draws a transition screen between players' turns
void GameUI::DrawPlayerTransitionScreen(const std::string &nextPlayerName)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int buttonWidth = 200;
    int buttonHeight = 60;

    // Dark overlay to dim the background
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 200});

    // Message box for the transition message
    DrawRectangle(screenWidth / 2 - 250, screenHeight / 2 - 100, 500, 200, (Color){0, 0, 128, 255});         // Dark blue background
    DrawRectangleLines(screenWidth / 2 - 250, screenHeight / 2 - 100, 500, 200, (Color){65, 105, 225, 255}); // Royal blue border

    char message[64];
    sprintf(message, "Pass device to %s", nextPlayerName.c_str());
    DrawText(message,
             screenWidth / 2 - MeasureText(message, 30) / 2,
             screenHeight / 2 - 60, 30, WHITE);

    // "I am ready" continue button
    Rectangle continueButton = {(float)(screenWidth / 2 - buttonWidth / 2), (float)(screenHeight / 2 + 20), (float)buttonWidth, (float)buttonHeight};
    bool continueHover = CheckCollisionPointRec(GetMousePosition(), continueButton);

    DrawRectangleRec(continueButton, continueHover ? (Color){144, 238, 144, 255} : (Color){60, 179, 113, 255});
    DrawText("I am ready", screenWidth / 2 - MeasureText("I am ready", 30) / 2, (int)(continueButton.y + buttonHeight / 2 - 15), 30, BLACK);

    // If continue button is clicked, dismiss transition screen and reset turn-related flags
    if (continueHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        awaitingPlayerChange = false;
        cardDrawnThisTurn = false; // Reset for new player's turn
        showContinueButton = false;
        effectsApplied = false; // Reset this when transitioning to a new player
    }

    // Update and draw the fullscreen toggle button
    UpdateFullscreenButton();
    DrawButton(fullscreenToggleButton);
}

// Requests the player to choose a color for a Wild or Draw Four card
void GameUI::RequestColorChoice(Card *card)
{
    // Check for null card pointer to prevent crashes
    if (!card) {
        throw Uno::NullPointerException("card in RequestColorChoice");
    }
    cardNeedingColorChoice = card; // Store the card that needs a color choice
    colorSelector.Show(); // Display the color selection UI
}

// Updates the position and handles clicks for the fullscreen toggle button
void GameUI::UpdateFullscreenButton()
{
    // Update position based on current screen dimensions
    fullscreenToggleButton.rect.x = GetScreenWidth() - 180;
    fullscreenToggleButton.rect.y = 10;

    // Check for hover and click
    Vector2 mousePos = GetMousePosition();
    fullscreenToggleButton.isHovered = CheckCollisionPointRec(mousePos, fullscreenToggleButton.rect);

    // Toggle fullscreen mode if the button is clicked
    if (fullscreenToggleButton.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        ToggleFullscreen();
    }
}

// Main function to handle the game screen rendering and user interactions
bool GameUI::HandleGameScreen(UnoGame &game)
{
    ClearBackground((Color){40, 0, 50, 255}); // Set a dark navy background

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Player *currentPlayer = nullptr;
    // Attempt to get the current player, handling potential exceptions
    try {
        currentPlayer = game.getCurrentPlayer();
        if (!currentPlayer) {
            throw Uno::PlayerException("Current player is null in HandleGameScreen");
        }
    } catch (const Uno::PlayerException& e) {
        SetStatusMessage(TextFormat("Player Error: %s", e.what()));
        return false; // Cannot proceed without a current player
    } catch (const Uno::GameStateException& e) {
        SetStatusMessage(TextFormat("Game State Error: %s", e.what()));
        return false;
    }


    // Check if the game is over; if so, draw the end game screen
    if (game.isGameOver())
    {
        if (!viewingEndScreen)
            viewingEndScreen = true;
        DrawEndGameScreen(game);
        return true; // Still updating the game screen (end screen)
    }

    // Handle color selector if it's active (for Wild/DrawFour cards)
    if (colorSelector.IsActive())
    {
        if (colorSelector.Update())
        {
            // A color was selected
            if (cardNeedingColorChoice)
            {
                try {
                    cardNeedingColorChoice->setColor(colorSelector.GetSelectedColor()); // Set the chosen color
                    SetStatusMessage("Color selected!");
                    showContinueButton = true; // Show continue button after color selection
                    cardNeedingColorChoice = nullptr; // Clear the card needing color choice
                } catch (const Uno::CardException& e) {
                    SetStatusMessage(TextFormat("Card Error: %s", e.what()));
                }
            }
        }
        return true; // Keep processing color selector until a choice is made
    }

    // Handle player transition screen if awaiting player change
    if (awaitingPlayerChange)
    {
        DrawPlayerTransitionScreen(currentPlayer->getName());
        return true; // Keep displaying transition screen
    }

    // Apply any pending effects from the last turn's action cards (e.g., Skip, Reverse)
    if (!effectsApplied)
    {
        try {
            game.applyPendingEffects();
            effectsApplied = true;

            // The hasPendingSkipEffect method is commented out in UnoGame.h,
            // so this logic might need adjustment based on its re-introduction or alternative
            // if (game.hasPendingSkipEffect())
            // {
            //     awaitingPlayerChange = true;
            //     effectsApplied = false; // Reset for next transition
            //     return true;
            // }
        } catch (const Uno::GameStateException& e) {
            SetStatusMessage(TextFormat("Game State Error: %s", e.what()));
        }
    }

    // Draw player information and the top card on the discard pile
    DrawPlayerInfo(currentPlayer);
    try {
        DrawTopCard(game.getTopCard());
    } catch (const Uno::CardException& e) {
        SetStatusMessage(TextFormat("Card Error: %s", e.what()));
    }


    // Define and initialize game interaction buttons
    Button unoButton = {
        {screenWidth - 220.0f, screenHeight - 120.0f, 180.0f, 50.0f},
        "Call UNO!",
        (Color){255, 105, 180, 255}, // Hot pink
        (Color){255, 182, 193, 255}, // Light pink
        false};

    Button drawButton = {
        {screenWidth / 2 - 90.0f, screenHeight - 120.0f, 180.0f, 50.0f},
        "Draw Card",
        (Color){60, 179, 113, 255},  // Medium sea green
        (Color){144, 238, 144, 255}, // Light green
        false};

    Button quitButton = {
        {40.0f, screenHeight - 120.0f, 180.0f, 50.0f},
        "Main Menu",               // Changed from "Quit Game" to make purpose clearer
        (Color){255, 69, 0, 255},  // Orange red
        (Color){255, 140, 0, 255}, // Dark orange
        false};

    // Continue button for after drawing a card or playing a card
    Button continueButton = {
        {screenWidth / 2 - 90.0f, 350.0f, 180.0f, 50.0f},
        "End Turn",
        (Color){60, 179, 113, 255},  // Medium sea green
        (Color){144, 238, 144, 255}, // Light green
        false};

    // Vector to store card rectangles for click detection
    std::vector<Rectangle> cardRects;

    // Draw the current player's hand
    try {
        DrawPlayerHand(currentPlayer, cardRects);
    } catch (const Uno::UnoException& e) {
        SetStatusMessage(TextFormat("UI Error: %s", e.what()));
    }

    // Draw the current status message
    DrawText(statusMessage, screenWidth / 2 - MeasureText(statusMessage, 20) / 2, 220, 20, YELLOW);

    // Draw the main game buttons
    DrawGameButtons(unoButton, drawButton, quitButton);

    // Draw continue button if it's currently set to be shown
    if (showContinueButton)
    {
        DrawButton(continueButton);
    }

    // Get mouse position for input handling
    Vector2 mousePos = GetMousePosition();

    // Handle button clicks
    if (IsButtonClicked(unoButton, mousePos))
    {
        try {
            currentPlayer->callUNO(); // Player attempts to call UNO
            SetStatusMessage(currentPlayer->getHandSize() == 2 ? "UNO called successfully!" : "You must have exactly 2 cards to call UNO!");
        } catch (const Uno::PlayerException& e) {
            SetStatusMessage(TextFormat("Player Error: %s", e.what()));
        }
    }
    else if (IsButtonClicked(drawButton, mousePos))
    {
        try {
            game.drawCard(currentPlayer); // Player draws a card
            SetStatusMessage("Card drawn. Look at your new card and end your turn when ready.");
            showContinueButton = true; // Show continue button after drawing
            cardDrawnThisTurn = true; // Mark that a card was drawn this turn
        } catch (const Uno::CardException& e) {
            SetStatusMessage(TextFormat("Card Error: %s", e.what()));
        } catch (const Uno::GameStateException& e) {
            SetStatusMessage(TextFormat("Game State Error: %s", e.what()));
        }
    }
    else if (showContinueButton && IsButtonClicked(continueButton, mousePos))
    {
        // Player has seen their drawn card/played a card and wants to end turn
        try {
            game.nextTurn(); // Advance to the next turn
            awaitingPlayerChange = true; // Go to transition screen
            showContinueButton = false; // Hide continue button
            cardDrawnThisTurn = false; // Reset card drawn status
            SetStatusMessage(""); // Clear status message
            effectsApplied = false; // Reset effects applied for the next player
        } catch (const Uno::GameStateException& e) {
            SetStatusMessage(TextFormat("Game State Error: %s", e.what()));
        }
    }
    else if (IsButtonClicked(quitButton, mousePos))
    {
        // Set exit flag to return to main menu
        exitRequested = true;
        SetStatusMessage("Returning to main menu...");
        return true;
    }

    // Check for card clicks (only if continue button is not showing, meaning player can still interact with hand)
    if (!showContinueButton)
    {
        for (int i = 0; i < cardRects.size(); i++)
        {
            if (CheckCollisionPointRec(mousePos, cardRects[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Card *selectedCard = nullptr;
                // Attempt to get the selected card, handling potential exceptions
                try {
                    selectedCard = currentPlayer->getCardAtIndex(i);
                    if (!selectedCard) {
                        throw Uno::CardException("Selected card is null after retrieval.");
                    }
                } catch (const Uno::PlayerException& e) {
                    SetStatusMessage(TextFormat("Player Error: %s", e.what()));
                    continue; // Skip to next card if error occurs
                } catch (const Uno::CardException& e) {
                    SetStatusMessage(TextFormat("Card Error: %s", e.what()));
                    continue; // Skip to next card if error occurs
                }

                // If already in DropTwo selection mode
                if (selectingCardsToDrop)
                {
                    // Allow selecting cards to drop
                    if (std::find(selectedCardIndices.begin(), selectedCardIndices.end(), i) == selectedCardIndices.end())
                    {
                        selectedCardIndices.push_back(i); // Add card to selected list
                        SetStatusMessage(TextFormat("Selected card %d to drop. Select %d more.", i, 2 - (int)selectedCardIndices.size()));
                    }
                    else
                    {
                        SetStatusMessage("Card already selected! Choose another.");
                    }
                    // If 2 cards are selected, drop them
                    if (selectedCardIndices.size() == 2)
                    {
                        // Drop the selected cards using UnoGame's dropCardFromPlayer
                        // Drop in descending order to avoid index shifts when removing from hand
                        std::sort(selectedCardIndices.begin(), selectedCardIndices.end(), std::greater<int>());
                        for (int index : selectedCardIndices)
                        {
                            try {
                                game.dropCardFromPlayer(currentPlayer, index);
                            } catch (const Uno::CardException& e) {
                                SetStatusMessage(TextFormat("Card Error: %s", e.what()));
                            } catch (const Uno::PlayerException& e) {
                                SetStatusMessage(TextFormat("Player Error: %s", e.what()));
                            } catch (const Uno::InvalidInputException& e) {
                                SetStatusMessage(TextFormat("Invalid Input Error: %s", e.what()));
                            }
                        }
                        selectingCardsToDrop = false; // Exit drop selection mode
                        selectedCardIndices.clear(); // Clear selected indices
                        SetStatusMessage("Dropped 2 cards. Your turn is over.");
                        showContinueButton = true; // Show continue button to end turn
                    }
                    continue; // Consume this click handling and do not attempt to play any card
                }

                // Attempt to play the selected card
                try {
                    if (game.isCardPlayable(selectedCard))
                    {
                        // Handle special cards (Wild, DrawFour, DropTwo)
                        if (selectedCard->getName() == "Wild" || selectedCard->getName() == "DrawFour")
                        {
                            currentPlayer->playCard(i, game); // Play the card
                            RequestColorChoice(game.getTopCard()); // Request color choice from player
                            SetStatusMessage("Choose a color for your Wild card");
                            game.enforceUNOCall(currentPlayer); // Enforce UNO call rules
                        }
                        else if (selectedCard->getName() == "DropTwo")
                        {
                            Card *dropTwoCard = selectedCard; // Get a copy of the selected card
                            currentPlayer->removeCardFromHand(i); // Remove the card from player's hand
                            game.setTopCard(dropTwoCard); // Manually set it as the top card
                            game.enforceUNOCall(currentPlayer); // Check for UNO call
                            selectingCardsToDrop = true; // Enter card selection mode for dropping
                            selectedCardIndices.clear();
                            SetStatusMessage("Drop Two played! Select up to 2 cards to drop.");
                        }
                        else
                        {
                            currentPlayer->playCard(i, game); // Play the card normally
                            game.enforceUNOCall(currentPlayer); // Check for UNO call

                            // Check if the current player has won
                            if (currentPlayer->hasWon())
                            {
                                SetStatusMessage(TextFormat("%s wins!", currentPlayer->getName().c_str()));
                                viewingEndScreen = true; // Go to end game screen
                                return true;
                            }
                            else
                            {
                                SetStatusMessage("Card played. Your turn is over.");
                                showContinueButton = true; // Show continue button after playing a card
                            }
                        }
                    }
                    else
                    {
                        SetStatusMessage("This card cannot be played! Try another or draw.");
                    }
                } catch (const Uno::CardException& e) {
                    SetStatusMessage(TextFormat("Card Play Error: %s", e.what()));
                } catch (const Uno::PlayerException& e) {
                    SetStatusMessage(TextFormat("Player Error: %s", e.what()));
                } catch (const Uno::GameStateException& e) {
                    SetStatusMessage(TextFormat("Game State Error: %s", e.what()));
                } catch (const Uno::InvalidInputException& e) {
                    SetStatusMessage(TextFormat("Invalid Input Error: %s", e.what()));
                }
            }
        }
    }

    return true; // Continue updating the game screen
}
