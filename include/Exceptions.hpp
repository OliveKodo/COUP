#pragma once

#include <exception>
#include <string>

namespace coup {

class GameException : public std::exception {
private:
    std::string _message;
    
public:
    GameException(const std::string& msg) : _message(msg) {}
    
    virtual const char* what() const noexcept override {
        return _message.c_str();
    }
};

class IllegalMoveException : public GameException {
public:
    IllegalMoveException(const std::string& msg = "Illegal move!") 
        : GameException(msg) {}
};

class NotEnoughCoinsException : public GameException {
public:
    NotEnoughCoinsException(const std::string& msg = "Not enough coins to perform this action!") 
        : GameException(msg) {}
};

class PlayerNotFoundException : public GameException {
public:
    PlayerNotFoundException(const std::string& msg = "Player not found!") 
        : GameException(msg) {}
};

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

class NotYourTurnException : public GameException {
public:
    NotYourTurnException(const std::string& msg = "It's not your turn!") 
        : GameException(msg) {}
};

class PlayerAlreadyInGameException : public GameException {
public:
    PlayerAlreadyInGameException(const std::string& msg = "Player with this name is already in the game!") 
        : GameException(msg) {}
};

class TooManyCoinsException : public GameException {
public:
    TooManyCoinsException(const std::string& msg = "Player has 10 or more coins and must perform coup!") 
        : GameException(msg) {}
};

class PlayerNotActiveException : public GameException {
public:
    PlayerNotActiveException(const std::string& msg = "This player is not active!") 
        : GameException(msg) {}
};

class SanctionedPlayerException : public GameException {
public:
    SanctionedPlayerException(const std::string& msg = "This player is under sanction and cannot perform this action!") 
        : GameException(msg) {}
};

} // namespace coup