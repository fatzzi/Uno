#include <raylib.h>
#include <cstring> // For strcpy
#include <cstdio>  // For fprintf
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include "../header/UnoGame.h" 
#include "../header/Player.h"
#include "../header/GameUI.h"
#include "../header/Exceptions.h" // Include custom exception header

// Enum to define different states of the game
enum GameState
{
    MAIN_MENU,    // Main menu screen
    HOW_TO_PLAY,  // How to play rules screen
    PLAYER_SETUP, // Player name input and setup screen
    GAME_SCREEN,  // Main game play screen
    END_SCREEN    // Game over screen
};

// Main function where the game execution begins
int main()
{
    int screenWidth = 800;  // Initial screen width
    int screenHeight = 600; // Initial screen height

    try
    {
        // Initialize Raylib window
        InitWindow(screenWidth, screenHeight, "UNO Game");
        SetTargetFPS(60); // Set desired framerate

        const int buttonWidth = 200;
        const int buttonHeight = 60;

        // Define colors for UI elements
        Color startColor = (Color){255, 105, 180, 255};     // Hot pink
        Color stopColor = (Color){255, 69, 0, 255};         // Orange red
        Color howToColor = (Color){60, 179, 113, 255};      // Medium sea green
        Color fullScreenColor = (Color){65, 105, 225, 255}; // Royal blue

        Color startHoverColor = (Color){255, 182, 193, 255};      // Light pink
        Color stopHoverColor = (Color){255, 140, 0, 255};         // Dark orange
        Color howToHoverColor = (Color){144, 238, 144, 255};      // Light green
        Color fullScreenHoverColor = (Color){100, 149, 237, 255}; // Cornflower blue

        bool exitProgram = false; // Flag to control program exit
        char statusMessage[64] = "Waiting for input..."; // Message displayed on screen
        GameState currentScreen = MAIN_MENU; // Initial game state

        int numPlayers = 0; // Number of players in the game
        std::vector<std::string> playerNames; // Stores confirmed player names
        std::vector<std::string> nameInputBuffer; // Buffer for player name input
        int activeNameIndex = 0; // Index of the player name currently being input

        UnoGame game;  // Create the game instance
        GameUI gameUI; // Create the UI controller

        // Main game loop: continues until window is closed or exit is requested
        while (!WindowShouldClose() && !exitProgram)
        {
            // Update screen dimensions in case of window resize
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();

            // Define button rectangles for main menu
            Rectangle startButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f - 120, (float)buttonWidth, (float)buttonHeight};
            Rectangle howToPlayButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f - 40, (float)buttonWidth, (float)buttonHeight};
            Rectangle stopButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 40, (float)buttonWidth, (float)buttonHeight};
            Rectangle fullScreenButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 120, (float)buttonWidth, (float)buttonHeight};
            Rectangle backButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 160, (float)buttonWidth, (float)buttonHeight};
            Rectangle mainMenuButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 160, (float)buttonWidth, (float)buttonHeight};

            Vector2 mouse = GetMousePosition(); // Get current mouse position

            // Check if mouse is hovering over buttons
            bool startHover = CheckCollisionPointRec(mouse, startButton);
            bool stopHover = CheckCollisionPointRec(mouse, stopButton);
            bool howToHover = CheckCollisionPointRec(mouse, howToPlayButton);
            bool fullScreenHover = CheckCollisionPointRec(mouse, fullScreenButton);
            bool backHover = CheckCollisionPointRec(mouse, backButton);
            bool mainMenuHover = CheckCollisionPointRec(mouse, mainMenuButton);

            // Check if GameUI requested an exit (e.g., from end screen)
            if (gameUI.IsExiting())
            {
                // Return to main menu instead of exiting program
                currentScreen = MAIN_MENU;
                // Reset game if returning from game screen to ensure a fresh start
                game = UnoGame(); // Create a new game instance, effectively resetting it
                gameUI = GameUI(); // Creates new UI, old one is destroyed
                numPlayers = 0;
                nameInputBuffer.clear();
                playerNames.clear();
            }

            // Handle mouse clicks based on current game state
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (currentScreen == MAIN_MENU)
                {
                    if (startHover)
                    {
                        strcpy(statusMessage, "Started!");
                        currentScreen = PLAYER_SETUP; // Transition to player setup
                    }
                    else if (howToHover)
                    {
                        currentScreen = HOW_TO_PLAY; // Transition to how to play screen
                        strcpy(statusMessage, "Showing Rules");
                    }
                    else if (stopHover)
                    {
                        exitProgram = true; // Set flag to exit program
                    }
                    else if (fullScreenHover)
                    {
                        ToggleFullscreen(); // Toggle fullscreen mode
                    }
                }
                else if (currentScreen == HOW_TO_PLAY)
                {
                    if (backHover)
                    {
                        currentScreen = MAIN_MENU; // Return to main menu
                    }
                }
                else if (currentScreen == PLAYER_SETUP)
                {
                    if (numPlayers == 0) // If number of players not yet selected
                    {
                        const int choices = 3; // Options for 2, 3, or 4 players
                        Rectangle numButtons[choices];
                        for (int i = 0; i < choices; i++)
                        {
                            numButtons[i] = Rectangle{
                                screenWidth / 2.0f - buttonWidth / 2.0f,
                                150.0f + i * (buttonHeight + 20.0f),
                                (float)buttonWidth,
                                (float)buttonHeight};
                            bool hover = CheckCollisionPointRec(mouse, numButtons[i]);
                            if (hover)
                            {
                                numPlayers = i + 2; // Set number of players (2, 3, or 4)
                                nameInputBuffer.clear();
                                nameInputBuffer.resize(numPlayers, ""); // Resize buffer for names
                                playerNames.clear();
                                strcpy(statusMessage, "Enter player names");
                                activeNameIndex = 0; // Start with first player's name input
                            }
                        }
                    }
                    else // If number of players selected, handle "Start Game" button
                    {
                        Rectangle startGameButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight - 100.0f, (float)buttonWidth, (float)buttonHeight};
                        bool startGameHover = CheckCollisionPointRec(mouse, startGameButton);
                        if (startGameHover)
                        {
                            bool allNamed = true;
                            for (int i = 0; i < numPlayers; i++)
                            {
                                if (nameInputBuffer[i].empty())
                                {
                                    allNamed = false; // Check if all player names are entered
                                    break;
                                }
                            }
                            if (allNamed)
                            {
                                playerNames = nameInputBuffer;
                                // Add players to the game instance
                                for (const auto &name : playerNames)
                                {
                                    Player *p = new Player(name);
                                    if (!p)
                                    {
                                        // Invoked if memory allocation for a player fails
                                        throw Uno::ResourceException("Player creation failed: out of memory.");
                                    }
                                    game.addPlayer(p); // Add player to the game
                                }
                                currentScreen = GAME_SCREEN; // Transition to game screen
                                strcpy(statusMessage, "Game Starting!");

                                // Start the game logic (deal cards, set top card)
                                game.startGame();
                            } else {
                                strcpy(statusMessage, "Please enter all player names!");
                            }
                        }
                    }
                }
                else if (currentScreen == END_SCREEN)
                {
                    if (mainMenuHover)
                    {
                        currentScreen = MAIN_MENU; // Return to main menu from end screen
                        // Reset game state for a new game
                        game = UnoGame();
                        gameUI = GameUI(); // reset the UI
                        numPlayers = 0;
                        nameInputBuffer.clear();
                        playerNames.clear();
                    }
                }
            }

            // Handle typing for player names during PLAYER_SETUP
            if (currentScreen == PLAYER_SETUP && numPlayers > 0)
            {
                int key = GetCharPressed(); // Get character pressed
                // Allow typing if key is valid ASCII and name length is within limits
                if (key > 0 && key < 128 && nameInputBuffer[activeNameIndex].length() < 12)
                {
                    nameInputBuffer[activeNameIndex] += (char)key;
                }
                // Handle backspace
                if (IsKeyPressed(KEY_BACKSPACE) && !nameInputBuffer[activeNameIndex].empty())
                {
                    nameInputBuffer[activeNameIndex].pop_back();
                }
                // Handle Enter key to move to next player's name input
                if (IsKeyPressed(KEY_ENTER))
                {
                    activeNameIndex = (activeNameIndex + 1) % numPlayers;
                }
            }

            // Drawing section
            BeginDrawing();

            // Draw appropriate screen based on current game state
            if (currentScreen == MAIN_MENU)
            {
                ClearBackground(BLACK); // Black background
                DrawText("UNO Main Menu", screenWidth / 2 - MeasureText("UNO Main Menu", 40) / 2, 80, 40, WHITE);
                // Draw main menu buttons
                DrawRectangleRec(startButton, startHover ? startHoverColor : startColor);
                DrawText("Start", screenWidth / 2 - MeasureText("Start", 30) / 2, (int)(startButton.y + buttonHeight / 2 - 15), 30, BLACK);
                DrawRectangleRec(howToPlayButton, howToHover ? howToHoverColor : howToColor);
                DrawText("How to Play", screenWidth / 2 - MeasureText("How to Play", 30) / 2, (int)(howToPlayButton.y + buttonHeight / 2 - 15), 30, BLACK);
                DrawRectangleRec(stopButton, stopHover ? stopHoverColor : stopColor);
                DrawText("Exit", screenWidth / 2 - MeasureText("Exit", 30) / 2, (int)(stopButton.y + buttonHeight / 2 - 15), 30, BLACK);
                DrawRectangleRec(fullScreenButton, fullScreenHover ? fullScreenHoverColor : fullScreenColor);
                DrawText("Fullscreen", screenWidth / 2 - MeasureText("Fullscreen", 30) / 2, (int)(fullScreenButton.y + buttonHeight / 2 - 15), 30, BLACK);
                DrawText(statusMessage, screenWidth / 2 - MeasureText(statusMessage, 20) / 2, screenHeight - 50, 20, GRAY);
            }
            else if (currentScreen == HOW_TO_PLAY)
            {
                ClearBackground(BLACK); // Black background
                const char *rules[] = { // Array of rules text
                    "UNO Rules:",
                    "- Match the top card by color or number.",
                    "- Use special cards: Skip, Reverse, Draw Two, Wild, Draw Four, Draw six, Drop 2.",
                    "- Call UNO when 2 cards left. First to discard all cards wins!","- If you havw 20+ cards, you are OUT!",
                    "- Say 'UNO' when you have two cards left!"};
                int y = screenHeight / 4 - 80;
                for (int i = 0; i < 5; i++)
                {
                    DrawText(rules[i], screenWidth / 2 - MeasureText(rules[i], 20) / 2, y, 20, WHITE);
                    y += 22; // Line spacing
                }
                // Draw back button
                DrawRectangleRec(backButton, backHover ? howToHoverColor : stopColor);
                DrawText("Back", screenWidth / 2 - MeasureText("Back", 30) / 2, (int)(backButton.y + buttonHeight / 2 - 15), 30, BLACK);
            }
            else if (currentScreen == PLAYER_SETUP)
            {
                ClearBackground(BLACK); // Black background
                if (numPlayers == 0) // If number of players not yet chosen
                {
                    const int choices = 3;
                    Rectangle numButtons[choices];
                    const char *labels[choices] = {"2 Players", "3 Players", "4 Players"};
                    for (int i = 0; i < choices; i++)
                    {
                        numButtons[i] = Rectangle{
                            screenWidth / 2.0f - buttonWidth / 2.0f,
                            150.0f + i * (buttonHeight + 20.0f),
                            (float)buttonWidth,
                            (float)buttonHeight};
                        bool hover = CheckCollisionPointRec(mouse, numButtons[i]);
                        DrawRectangleRec(numButtons[i], hover ? howToHoverColor : howToColor);
                        DrawText(labels[i], screenWidth / 2 - MeasureText(labels[i], 30) / 2,
                                 (int)(numButtons[i].y + buttonHeight / 2 - 15), 30, BLACK);
                    }
                }
                else // If number of players chosen, display name input fields
                {
                    DrawText("Enter player names:", screenWidth / 2 - MeasureText("Enter player names:", 30) / 2, 100, 30, WHITE);
                    char inputLabel[64];
                    for (int i = 0; i < numPlayers; i++)
                    {
                        // Display player name input field with cursor
                        sprintf(inputLabel, "Player %d: %s%s", i + 1, nameInputBuffer[i].c_str(), (i == activeNameIndex) ? "_" : "");
                        DrawText(inputLabel, screenWidth / 2 - MeasureText(inputLabel, 20) / 2, 150 + i * 40, 20, (i == activeNameIndex) ? YELLOW : WHITE);
                    }
                    bool allNamed = true;
                    for (int i = 0; i < numPlayers; i++)
                    {
                        if (nameInputBuffer[i].empty())
                        {
                            allNamed = false;
                            break;
                        }
                    }
                    if (allNamed) // Only show "Start Game" button if all names are entered
                    {
                        Rectangle startGameButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight - 100.0f, (float)buttonWidth, (float)buttonHeight};
                        bool startGameHover = CheckCollisionPointRec(mouse, startGameButton);
                        DrawRectangleRec(startGameButton, startGameHover ? startHoverColor : startColor);
                        DrawText("Start Game", screenWidth / 2 - MeasureText("Start Game", 30) / 2, (int)(startGameButton.y + buttonHeight / 2 - 15), 30, BLACK);
                    }
                }
            }
            else if (currentScreen == GAME_SCREEN)
            {
                try
                {
                    // Use GameUI class to handle the game screen rendering and logic
                    if (!gameUI.HandleGameScreen(game))
                    {
                        // Invoked if HandleGameScreen returns false, indicating a critical GUI error
                        throw Uno::GuiException("Game screen handling failed");
                    }
                }
                catch (const Uno::UnoException &e)
                {
                    // Catch any custom UNO exceptions thrown during game screen handling
                    strcpy(statusMessage, e.what()); // Display error message
                    currentScreen = MAIN_MENU; // Return to main menu on error
                }
                catch (const std::exception &e)
                {
                    // Catch any other standard exceptions
                    strcpy(statusMessage, TextFormat("Unexpected Error: %s", e.what()));
                    currentScreen = MAIN_MENU; // Return to main menu on error
                }
            }

            EndDrawing(); // End drawing frame
        }

        CloseWindow(); // Close Raylib window
        return 0;      // Program exits successfully
    }
    catch (const Uno::UnoException &e)
    {
        // Catch any custom UNO exceptions that propagate up to main
        std::cerr << "UNO Game Error: " << e.what() << std::endl;

        // If window is already initialized, show error message on screen
        if (IsWindowReady())
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Error in UNO Game", screenWidth / 2 - MeasureText("Error in UNO Game", 30) / 2, screenHeight / 2 - 60, 30, RED);
            DrawText(e.what(), screenWidth / 2 - MeasureText(e.what(), 20) / 2, screenHeight / 2, 20, WHITE);
            DrawText("Press any key to exit", screenWidth / 2 - MeasureText("Press any key to exit", 20) / 2, screenHeight / 2 + 60, 20, GRAY);
            EndDrawing();

            // Wait for key press before closing the window
            while (!WindowShouldClose() && !GetKeyPressed())
            {
            }
            CloseWindow();
        }

        return 1; // Indicate error exit
    }
    catch (const std::exception &e)
    {
        // Catch any other standard exceptions that propagate up to main
        std::cerr << "Standard Exception: " << e.what() << std::endl;
        return 1; // Indicate error exit
    }
    catch (...)
    {
        // Catch any unknown exceptions
        std::cerr << "Unknown Exception Occurred" << std::endl;
        return 1; // Indicate error exit
    }
}
