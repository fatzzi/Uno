#include <exception>
#include <stdexcept>
#include "..\header\Exceptions.h"

namespace Uno
{

    // Base UnoException
    UnoException::UnoException(const std::string &msg) : message(msg) {}

    const char *UnoException::what() const noexcept
    {
        return message.c_str();
    }

    // NullPointerException implementation
    NullPointerException::NullPointerException(const std::string &objectName)
        : UnoException("Null pointer error: " + objectName + " is null") {}

    // PlayerException implementation
    PlayerException::PlayerException(const std::string &msg)
        : UnoException("Player error: " + msg) {}

    // CardException implementation
    CardException::CardException(const std::string &msg)
        : UnoException("Card error: " + msg) {}

    // GameStateException implementation
    GameStateException::GameStateException(const std::string &msg)
        : UnoException("Game state error: " + msg) {}

    // GuiException implementation
    GuiException::GuiException(const std::string &msg)
        : UnoException("GUI error: " + msg) {}

    // ResourceException implementation
    ResourceException::ResourceException(const std::string &msg)
        : UnoException("Resource error: " + msg) {}

    // InvalidInputException implementation
    InvalidInputException::InvalidInputException(const std::string &msg)
        : UnoException("Invalid input: " + msg) {}

}