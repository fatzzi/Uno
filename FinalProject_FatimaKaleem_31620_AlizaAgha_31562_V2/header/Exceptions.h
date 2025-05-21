#ifndef UNO_EXCEPTION_H
#define UNO_EXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>

namespace Uno {

// Base exception class for all UNO game exceptions
class UnoException : public std::exception {
protected:
    std::string message;

public:
    UnoException(const std::string& msg);
    virtual ~UnoException() noexcept = default;
    virtual const char* what() const noexcept override;
};

// Exception for null pointers
class NullPointerException : public UnoException {
public:
    NullPointerException(const std::string& objectName);
};

// Exception for player errors
class PlayerException : public UnoException {
public:
    PlayerException(const std::string& msg);
};

// Exception for card errors
class CardException : public UnoException {
public:
    CardException(const std::string& msg);
};

// Exception for game state errors
class GameStateException : public UnoException {
public:
    GameStateException(const std::string& msg);
};

// Exception for GUI errors
class GuiException : public UnoException {
public:
    GuiException(const std::string& msg);
};

// Exception for file/resource errors
class ResourceException : public UnoException {
public:
    ResourceException(const std::string& msg);
};

// Exception for invalid input
class InvalidInputException : public UnoException {
public:
    InvalidInputException(const std::string& msg);
};

} // namespace Uno

#endif // UNO_EXCEPTION_H