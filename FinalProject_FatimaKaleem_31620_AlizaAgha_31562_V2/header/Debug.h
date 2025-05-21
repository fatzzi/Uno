/*#ifndef CARD_UTILS_H
#define CARD_UTILS_H

template <typename T1, typename T2>
bool areCardsPlayable(const T1* card1, const T2* card2) {
    if (!card1 || !card2) return false;
    return *card1 == *card2;
}

#endif
*/

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>

// A simple generic template function that prints variable name and value.
// Does not modify input, just prints for debugging purposes.
template <typename T>
void debug(const std::string& varName, const T& value) {
    std::cout << "Debug: " << varName << " = " << value << std::endl;
}

#endif 
