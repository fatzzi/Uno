// Color.h
#ifndef COLOR_H
#define COLOR_H
#include<string>    
using namespace std;
enum class Color {
    NONE,
    RED,
    YELLOW,
    GREEN,
    BLUE
};

// Function to convert Color enum to string representation
std::string colorToString(Color color) {
    switch (color) {
        case Color::RED: return "Red";
        case Color::YELLOW: return "Yellow";
        case Color::GREEN: return "Green";
        case Color::BLUE: return "Blue";
        default: return "None";
    }
}

#endif // COLOR_H
