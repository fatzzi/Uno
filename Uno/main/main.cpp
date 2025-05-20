#include <raylib.h>
#include <cstring> // For strcpy
#include <cstdio>  // For fprintf
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include "../header/UnoGame.h" // Adjusted path
#include "../header/Player.h"
#include "../header/GameUI.h"
#include "../header/Exceptions.h"

enum GameState
{
    MAIN_MENU,
    HOW_TO_PLAY,
    PLAYER_SETUP,
    GAME_SCREEN,
    END_SCREEN
};

int main()
{
    int screenWidth = 800;
    int screenHeight = 600;

    try
    {
        InitWindow(screenWidth, screenHeight, "UNO Game");
        SetTargetFPS(60);

        const int buttonWidth = 200;
        const int buttonHeight = 60;

        // Colors
        Color startColor = (Color){255, 105, 180, 255};     // Hot pink
        Color stopColor = (Color){255, 69, 0, 255};         // Orange red
        Color howToColor = (Color){60, 179, 113, 255};      // Medium sea green
        Color fullScreenColor = (Color){65, 105, 225, 255}; // Royal blue

        Color startHoverColor = (Color){255, 182, 193, 255};      // Light pink
        Color stopHoverColor = (Color){255, 140, 0, 255};         // Dark orange
        Color howToHoverColor = (Color){144, 238, 144, 255};      // Light green
        Color fullScreenHoverColor = (Color){100, 149, 237, 255}; // Cornflower blue

        bool exitProgram = false;
        char statusMessage[64] = "Waiting for input...";
        GameState currentScreen = MAIN_MENU;

        int numPlayers = 0;
        std::vector<std::string> playerNames;
        std::vector<std::string> nameInputBuffer;
        int activeNameIndex = 0;

        UnoGame game;  // Create the game instance
        GameUI gameUI; // Create the UI controller

        // Main game loop
        while (!WindowShouldClose() && !exitProgram)
        {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();

            Rectangle startButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f - 120, (float)buttonWidth, (float)buttonHeight};
            Rectangle howToPlayButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f - 40, (float)buttonWidth, (float)buttonHeight};
            Rectangle stopButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 40, (float)buttonWidth, (float)buttonHeight};
            Rectangle fullScreenButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 120, (float)buttonWidth, (float)buttonHeight};
            Rectangle backButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 160, (float)buttonWidth, (float)buttonHeight};
            Rectangle mainMenuButton = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 160, (float)buttonWidth, (float)buttonHeight};

            Vector2 mouse = GetMousePosition();

            bool startHover = CheckCollisionPointRec(mouse, startButton);
            bool stopHover = CheckCollisionPointRec(mouse, stopButton);
            bool howToHover = CheckCollisionPointRec(mouse, howToPlayButton);
            bool fullScreenHover = CheckCollisionPointRec(mouse, fullScreenButton);
            bool backHover = CheckCollisionPointRec(mouse, backButton);
            bool mainMenuHover = CheckCollisionPointRec(mouse, mainMenuButton);

            // Check if GameUI requested an exit
            if (gameUI.IsExiting())
            {
                // Return to main menu instead of exiting program
                currentScreen = MAIN_MENU;
                // Reset game if returning from game screen
                game = UnoGame(); // Create a new game instance
                numPlayers = 0;
                nameInputBuffer.clear();
                playerNames.clear();
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (currentScreen == MAIN_MENU)
                {
                    if (startHover)
                    {
                        strcpy(statusMessage, "Started!");
                        currentScreen = PLAYER_SETUP;
                    }
                    else if (howToHover)
                    {
                        currentScreen = HOW_TO_PLAY;
                        strcpy(statusMessage, "Showing Rules");
                    }
                    else if (stopHover)
                    {
                        exitProgram = true;
                    }
                    else if (fullScreenHover)
                    {
                        ToggleFullscreen();
                    }
                }
                else if (currentScreen == HOW_TO_PLAY)
                {
                    if (backHover)
                    {
                        currentScreen = MAIN_MENU;
                    }
                }
                else if (currentScreen == PLAYER_SETUP)
                {
                    if (numPlayers == 0)
                    {
                        const int choices = 3;
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
                                numPlayers = i + 2;
                                nameInputBuffer.clear();
                                nameInputBuffer.resize(numPlayers, "");
                                playerNames.clear();
                                strcpy(statusMessage, "Enter player names");
                                activeNameIndex = 0;
                            }
                        }
                    }
                    else
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
                                    allNamed = false;
                                    break;
                                }
                            }
                            if (allNamed)
                            {
                                playerNames = nameInputBuffer;
                                // Add players to the game
                                for (const auto &name : playerNames)
                                {
                                    Player *p = new Player(name);
                                    if (!p)
                                    {
                                        throw Uno::NullPointerException("Player creation failed");
                                    }
                                    game.addPlayer(p);
                                }
                                currentScreen = GAME_SCREEN;
                                strcpy(statusMessage, "Game Starting!");

                                // Start the game
                                game.startGame();
                            }
                        }
                    }
                }
                else if (currentScreen == END_SCREEN)
                {
                    if (mainMenuHover)
                    {
                        currentScreen = MAIN_MENU;
                        // Reset game
                        game = UnoGame();
                        numPlayers = 0;
                        nameInputBuffer.clear();
                        playerNames.clear();
                    }
                }
            }

            // Handle typing for player names
            if (currentScreen == PLAYER_SETUP && numPlayers > 0)
            {
                int key = GetCharPressed();
                if (key > 0 && key < 128 && nameInputBuffer[activeNameIndex].length() < 12)
                {
                    nameInputBuffer[activeNameIndex] += (char)key;
                }
                if (IsKeyPressed(KEY_BACKSPACE) && !nameInputBuffer[activeNameIndex].empty())
                {
                    nameInputBuffer[activeNameIndex].pop_back();
                }
                if (IsKeyPressed(KEY_ENTER))
                {
                    activeNameIndex = (activeNameIndex + 1) % numPlayers;
                }
            }

            // Drawing
            BeginDrawing();

            // Draw appropriate screen based on game state
            if (currentScreen == MAIN_MENU)
            {
                ClearBackground(BLACK);
                DrawText("UNO Main Menu", screenWidth / 2 - MeasureText("UNO Main Menu", 40) / 2, 80, 40, WHITE);
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
                ClearBackground(BLACK);
                const char *rules[] = {
                    "UNO Rules:",
                    "- Match the top card by color or number.",
                    "- Use special cards: Skip, Reverse, Draw Two, Wild, Draw Four.",
                    "- First to discard all cards wins!",
                    "- Say 'UNO' when you have one card left!"};
                int y = screenHeight / 4 - 80;
                for (int i = 0; i < 5; i++)
                {
                    DrawText(rules[i], screenWidth / 2 - MeasureText(rules[i], 20) / 2, y, 20, WHITE);
                    y += 22;
                }
                DrawRectangleRec(backButton, backHover ? howToHoverColor : stopColor);
                DrawText("Back", screenWidth / 2 - MeasureText("Back", 30) / 2, (int)(backButton.y + buttonHeight / 2 - 15), 30, BLACK);
            }
            else if (currentScreen == PLAYER_SETUP)
            {
                ClearBackground(BLACK);
                if (numPlayers == 0)
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
                else
                {
                    DrawText("Enter player names:", screenWidth / 2 - MeasureText("Enter player names:", 30) / 2, 100, 30, WHITE);
                    char inputLabel[64];
                    for (int i = 0; i < numPlayers; i++)
                    {
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
                    if (allNamed)
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
                    // Use GameUI class to handle the game screen
                    if (!gameUI.HandleGameScreen(game))
                    {
                        throw Uno::GuiException("Game screen handling failed");
                    }
                }
                catch (const Uno::UnoException &e)
                {
                    strcpy(statusMessage, e.what());
                    currentScreen = MAIN_MENU; // Return to main menu on error
                }
            }

            EndDrawing();
        }

        CloseWindow();
        return 0;
    }
    catch (const Uno::UnoException &e)
    {
        std::cerr << "UNO Game Error: " << e.what() << std::endl;

        // If window is already initialized, show error message
        if (IsWindowReady())
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Error in UNO Game", screenWidth / 2 - MeasureText("Error in UNO Game", 30) / 2, screenHeight / 2 - 60, 30, RED);
            DrawText(e.what(), screenWidth / 2 - MeasureText(e.what(), 20) / 2, screenHeight / 2, 20, WHITE);
            DrawText("Press any key to exit", screenWidth / 2 - MeasureText("Press any key to exit", 20) / 2, screenHeight / 2 + 60, 20, GRAY);
            EndDrawing();

            // Wait for key press before closing
            while (!WindowShouldClose() && !GetKeyPressed())
            {
            }
            CloseWindow();
        }

        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Standard Exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown Exception Occurred" << std::endl;
        return 1;
    }
}
