#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>  // For debugging

namespace coup {

Game::Game() : _currentTurn(0), _kupah(0), _gameStarted(false) {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Game::addPlayer(std::shared_ptr<Player> player) {
    // Check if game has already started
    if (_gameStarted) {
        throw GameException("Cannot add player after game has started!");
    }
    
    // Check if we already have max players (6)
    if (_players.size() >= 6) {
        throw TooManyPlayersException();
    }
    
    // Check if player with this name already exists
    for (const auto& p : _players) {
        if (p->getName() == player->getName()) {
            throw PlayerAlreadyInGameException();
        }
    }
    
    // Add the player to the game
    _players.push_back(player);
    
    // Debug print
    std::cout << "Added player: " << player->getName() << " (Total: " << _players.size() << ")" << std::endl;
}

std::shared_ptr<Player> Game::addPlayerWithRandomRole(const std::string& name) {
    // Generate random number between 0 and 5
    int roleIndex = std::rand() % 6;
    
    std::shared_ptr<Player> player;
    
    switch (roleIndex) {
        case 0:
            player = std::make_shared<Governor>(*this, name);
            break;
        case 1:
            player = std::make_shared<Spy>(*this, name);
            break;
        case 2:
            player = std::make_shared<Baron>(*this, name);
            break;
        case 3:
            player = std::make_shared<General>(*this, name);
            break;
        case 4:
            player = std::make_shared<Judge>(*this, name);
            break;
        case 5:
            player = std::make_shared<Merchant>(*this, name);
            break;
        default:
            player = std::make_shared<Governor>(*this, name); // Fallback
    }
    
    addPlayer(player);
    return player;
}

void Game::removePlayer(const std::string& name) {
    auto it = std::find_if(_players.begin(), _players.end(), 
                         [&name](const std::shared_ptr<Player>& p) { 
                             return p->getName() == name; 
                         });
    
    if (it == _players.end()) {
        throw PlayerNotFoundException();
    }
    
    (*it)->setActive(false);
}

std::shared_ptr<Player> Game::getPlayer(const std::string& name) {
    auto it = std::find_if(_players.begin(), _players.end(), 
                         [&name](const std::shared_ptr<Player>& p) { 
                             return p->getName() == name; 
                         });
    
    if (it == _players.end()) {
        throw PlayerNotFoundException();
    }
    
    return *it;
}

std::shared_ptr<Player> Game::getCurrentPlayer() {
    if (_players.empty()) {
        throw GameException("No players in the game!");
    }
    
    return _players[static_cast<size_t>(_currentTurn)];
}

std::string Game::turn() const {
    if (_players.empty()) {
        throw GameException("No players in the game!");
    }
    
    // Return the name of the current active player
    return _players[static_cast<size_t>(_currentTurn)]->getName();
}

std::vector<std::string> Game::players() const {
    // Debug print
    std::cout << "Number of players in game: " << _players.size() << std::endl;
    
    std::vector<std::string> result;
    
    for (const auto& player : _players) {
        if (player->isActive()) {
            result.push_back(player->getName());
        }
    }
    
    return result;
}

std::string Game::winner() const {
    // Check if game is over (only one active player)
    if (countActivePlayers() != 1) {
        throw GameStillRunningException();
    }
    
    // Find the active player
    for (const auto& player : _players) {
        if (player->isActive()) {
            return player->getName();
        }
    }
    
    // Should not get here if countActivePlayers() == 1
    throw GameException("No winner found!");
}

void Game::startGame() {
    if (_players.size() < 2) {
        throw GameException("Need at least 2 players to start!");
    }
    
    _gameStarted = true;
    _currentTurn = 0;
    
    // Initialize the kupah with coins (e.g., 50 coins)
    _kupah = 50;
    
    // Make sure the first player is active
    while (static_cast<size_t>(_currentTurn) < _players.size() && 
           !_players[static_cast<size_t>(_currentTurn)]->isActive()) {
        _currentTurn++;
    }
    
    if (static_cast<size_t>(_currentTurn) >= _players.size()) {
        throw GameException("No active players to start the game!");
    }
}

bool Game::isGameOver() const {
    return countActivePlayers() <= 1;
}

void Game::nextTurn() {
    if (isGameOver()) {
        throw GameOverException();
    }
    
    // Find the next active player
    do {
        _currentTurn = (_currentTurn + 1) % static_cast<int>(_players.size());
    } while (!_players[static_cast<size_t>(_currentTurn)]->isActive());
    
    // Start the player's turn (for any start-of-turn effects)
    _players[static_cast<size_t>(_currentTurn)]->startTurn();
}

int Game::getKupah() const {
    return _kupah;
}

void Game::removeFromKupah(int amount) {
    if (_kupah < amount) {
        throw GameException("Not enough coins in the kupah!");
    }
    
    _kupah -= amount;
}

void Game::addToKupah(int amount) {
    _kupah += amount;
}

bool Game::isPlayerTurn(const std::string& playerName) const {
    if (_players.empty() || static_cast<size_t>(_currentTurn) >= _players.size()) {
        return false;
    }
    
    return _players[static_cast<size_t>(_currentTurn)]->getName() == playerName;
}

int Game::countActivePlayers() const {
    int count = 0;
    
    for (const auto& player : _players) {
        if (player->isActive()) {
            count++;
        }
    }
    
    return count;
}

// Methods for managing pending actions

void Game::addPendingAction(const std::string& playerName, const std::string& actionType, 
                           std::shared_ptr<Player> target, std::shared_ptr<Player> victim) {
    _pendingActions.push_back({playerName, actionType, target, victim});
}

bool Game::hasPendingAction(const std::string& playerName, const std::string& actionType) const {
    for (const auto& action : _pendingActions) {
        if (action.playerName == playerName && action.actionType == actionType) {
            return true;
        }
    }
    return false;
}

void Game::clearPendingAction(const std::string& playerName, const std::string& actionType) {
    _pendingActions.erase(
        std::remove_if(_pendingActions.begin(), _pendingActions.end(),
                      [&](const PendingAction& action) { 
                          return action.playerName == playerName && action.actionType == actionType; 
                      }),
        _pendingActions.end()
    );
}

void Game::clearAllPendingActions(const std::string& playerName) {
    _pendingActions.erase(
        std::remove_if(_pendingActions.begin(), _pendingActions.end(),
                      [&](const PendingAction& action) { 
                          return action.playerName == playerName; 
                      }),
        _pendingActions.end()
    );
}

std::vector<Game::PendingAction> Game::getPendingActions() const {
    return _pendingActions;
}

Game::PendingAction Game::getPendingAction(const std::string& playerName, const std::string& actionType) const {
    for (const auto& action : _pendingActions) {
        if (action.playerName == playerName && action.actionType == actionType) {
            return action;
        }
    }
    
    throw GameException("No pending action found!");
}

} // namespace coup