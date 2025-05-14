#include "../header/DrawSixCard.h"
#include "../header/UnoGame.h"
#include "../header/ColorUtils.h" // Required for cardColorToString and getRaylibColor
#include <iostream>

DrawSixCard::DrawSixCard(CardColor color)
    : ActionCard("DrawSix", color) {}

void DrawSixCard::play(UnoGame& game, Player* currentPlayer) {
    try {
        std::cout << "Draw Six Card played! Next player must draw 6 cards.\n";
        game.makeNextPlayerDraw(6);
    } catch (const std::exception& e) {
        std::cerr << "Exception in DrawSixCard::play(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in DrawSixCard::play()" << std::endl;
    }
}

void DrawSixCard::print() const {
    try {
        std::cout << cardColorToString(color) << " Draw Six";
    } catch (const std::exception& e) {
        std::cerr << "Exception in DrawSixCard::print(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in DrawSixCard::print()" << std::endl;
    }
}

std::string DrawSixCard::getName() const {
    return "DrawSix";
}

void DrawSixCard::DrawCard(int x, int y) const {
    try {
        Color rayColor = getRaylibColor(color);
        DrawRectangle(x, y, 60, 90, rayColor);
        DrawText("D6", x + 20, y + 35, 20, BLACK);
    } catch (const std::exception& e) {
        std::cerr << "Exception in DrawSixCard::DrawCard(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in DrawSixCard::DrawCard()" << std::endl;
    }
}
