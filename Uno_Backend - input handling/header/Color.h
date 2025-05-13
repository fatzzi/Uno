#ifndef COLOR_H
#define COLOR_H

#include <string>

enum class Color {
    RED,
    BLUE,
    GREEN,
    YELLOW,
    NONE  // For wild cards before color is chosen
};

// Helper function to convert Color to string
inline std::string colorToString(Color color) {
    switch (color) {
        case Color::RED: return "Red";
        case Color::BLUE: return "Blue";
        case Color::GREEN: return "Green";
        case Color::YELLOW: return "Yellow";
        case Color::NONE: return "None";
        default: return "Unknown";
    }
}

// Helper function to convert string to Color
inline Color stringToColor(const std::string& str) {
    if (str == "Red") return Color::RED;
    if (str == "Blue") return Color::BLUE;
    if (str == "Green") return Color::GREEN;
    if (str == "Yellow") return Color::YELLOW;
    return Color::NONE;
}

#endif // COLOR_H