#ifndef CARD_COLOR_H
#define CARD_COLOR_H

#include <string>

// Rename enum to CardColor to avoid conflict with Raylib's Color
enum class CardColor {
    Red,
    Blue,
    Green,
    Yellow,
    NONE  // For wild cards before color is chosen
};

// Helper function to convert CardColor to string
inline std::string cardColorToString(CardColor color) {
    switch (color) {
        case CardColor::Red: return "Red";
        case CardColor::Blue: return "Blue";
        case CardColor::Green: return "Green";
        case CardColor::Yellow: return "Yellow";
        case CardColor::NONE: return "None";
        default: return "Unknown";
    }
}

// Helper function to convert string to CardColor
inline CardColor stringToCardColor(const std::string& str) {
    if (str == "Red") return CardColor::Red;
    if (str == "Blue") return CardColor::Blue;
    if (str == "Green") return CardColor::Green;
    if (str == "Yellow") return CardColor::Yellow;
    return CardColor::NONE;
}

#endif // CARD_COLOR_H
