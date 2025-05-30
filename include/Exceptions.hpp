//tomergal40@gmail.com
#pragma once
#include <exception>
#include <string>

namespace coup {

// Base exception class for all game-related errors
class GameException : public std::exception {
private:
    std::string _message;
public:
    explicit GameException(const std::string& msg) : _message(msg) {}
    const char* what() const noexcept override { return _message.c_str(); }
};

// --- Game state errors ---
class GameStillRunningException : public GameException {
public:
    GameStillRunningException(const std::string& msg = "Game is still running, no winner yet!")
        : GameException(msg) {}
};

class GameOverException : public GameException {
public:
    GameOverException(const std::string& msg = "Game is already over!")
        : GameException(msg) {}
};

class TooManyPlayersException : public GameException {
public:
    TooManyPlayersException(const std::string& msg = "Too many players! Maximum is 6.")
        : GameException(msg) {}
};

// --- Player state errors ---
class PlayerAlreadyInGameException : public GameException {
public:
    PlayerAlreadyInGameException(const std::string& msg = "Player with this name is already in the game!")
        : GameException(msg) {}
};

class PlayerNotFoundException : public GameException {
public:
    PlayerNotFoundException(const std::string& msg = "Player not found!")
        : GameException(msg) {}
};

class PlayerNotActiveException : public GameException {
public:
    PlayerNotActiveException(const std::string& msg = "This player is not active!")
        : GameException(msg) {}
};

// --- Turn and move errors ---
class NotYourTurnException : public GameException {
public:
    NotYourTurnException(const std::string& msg = "It's not your turn!")
        : GameException(msg) {}
};

class IllegalMoveException : public GameException {
public:
    IllegalMoveException(const std::string& msg = "Illegal move!")
        : GameException(msg) {}
};

// --- Resource-related errors ---
class NotEnoughCoinsException : public GameException {
public:
    NotEnoughCoinsException(const std::string& msg = "Not enough coins to perform this action!")
        : GameException(msg) {}
};

class TooManyCoinsException : public GameException {
public:
    TooManyCoinsException(const std::string& msg = "Player has 10 or more coins and must perform a coup!")
        : GameException(msg) {}
};

// --- Sanction/Status errors ---
class SanctionedPlayerException : public GameException {
public:
    SanctionedPlayerException(const std::string& msg = "This player is under sanction and cannot perform this action!")
        : GameException(msg) {}
};

} // namespace coup
