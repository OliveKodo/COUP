#include "../include/Game.hpp"
#include "../include/Player.hpp" // Include Player.hpp before using Player methods
#include "../include/Exceptions.hpp"
#include <algorithm>
#include <iostream>

namespace coup {

Game::Game() : _currentTurn(0), _bank(100), _gameStarted(false) {}

void Game::addPlayer(std::shared_ptr<Player> player) {
    if (_gameStarted) throw GameException("Cannot add player after game has started!");
    if (_players.size() >= 6) throw TooManyPlayersException();
    for (const auto& p : _players)
        if (p->getName() == player->getName())
            throw PlayerAlreadyInGameException();
    _players.push_back(player);
    std::cout << "Added player: " << player->getName() << "\n";
}

void Game::removePlayer(const std::string& name) {
    auto it = std::find_if(_players.begin(), _players.end(), [&](auto& p){ return p->getName() == name; });
    if (it == _players.end()) throw PlayerNotFoundException();
    (*it)->setActive(false);
}

std::shared_ptr<Player> Game::getPlayer(const std::string& name) {
    for (auto& p : _players)
        if (p->getName() == name)
            return p;
    throw PlayerNotFoundException();
}

std::shared_ptr<Player> Game::getCurrentPlayer() {
    if (_players.empty()) throw GameException("No players in the game!");
    return _players.at(static_cast<size_t>(_currentTurn));
}

std::string Game::turn() const {
    if (_players.empty()) throw GameException("No players in the game!");
    return _players.at(static_cast<size_t>(_currentTurn))->getName();
}

std::vector<std::string> Game::players() const {
    std::vector<std::string> res;
    for (const auto& p : _players)
        if (p->isActive()) res.push_back(p->getName());
    return res;
}

std::string Game::winner() const {
    if (countActivePlayers() != 1) throw GameStillRunningException();
    for (const auto& p : _players)
        if (p->isActive()) return p->getName();
    throw GameException("No winner found!");
}

void Game::startGame() {
    if (_players.size() < 2) throw GameException("Need at least 2 players to start!");
    _gameStarted = true;
    _currentTurn = 0;
    
    // Fix: Add explicit cast to avoid signed/unsigned comparison
    while (static_cast<size_t>(_currentTurn) < _players.size() && 
           !_players[static_cast<size_t>(_currentTurn)]->isActive()) {
        _currentTurn++;
    }
    
    // Fix: Add explicit cast to avoid signed/unsigned comparison
    if (static_cast<size_t>(_currentTurn) >= _players.size()) {
        throw GameException("No active players to start!");
    }
}

bool Game::isGameOver() const {
    return countActivePlayers() <= 1;
}

void Game::nextTurn() {
    if (isGameOver()) {
        if (countActivePlayers() == 0) throw GameOverException();
        for (size_t i = 0; i < _players.size(); ++i)
            if (_players[i]->isActive()) { _currentTurn = static_cast<int>(i); return; }
    }

    int count = 0;
    // Fix: Remove unused variable
    // size_t start = _currentTurn; 
    
    do {
        _currentTurn = (_currentTurn + 1) % static_cast<int>(_players.size());
        ++count;
        
        // Fix: Add explicit cast to avoid signed/unsigned comparison
        if (static_cast<size_t>(count) > _players.size()) {
            throw GameException("No active players remaining!");
        }
    } while (!_players[static_cast<size_t>(_currentTurn)]->isActive());
    
    _players[static_cast<size_t>(_currentTurn)]->startTurn();
}

int Game::getBank() const { return _bank; }
void Game::removeFromBank(int amount) {
    if (_bank < amount) throw GameException("Not enough coins in bank!");
    _bank -= amount;
}
void Game::addToBank(int amount) { _bank += amount; }

bool Game::isPlayerTurn(const std::string& name) const {
    return !_players.empty() && _players[static_cast<size_t>(_currentTurn)]->getName() == name;
}

int Game::countActivePlayers() const {
    return std::count_if(_players.begin(), _players.end(), [](auto& p) { return p->isActive(); });
}

// Add the missing overloads of addPendingAction
void Game::addPendingAction(const std::string& playerName, const std::string& actionType) {
    _pendingActions.push_back({playerName, actionType, nullptr, nullptr});
}

void Game::addPendingAction(const std::string& playerName, const std::string& actionType,
                           std::shared_ptr<Player> target) {
    _pendingActions.push_back({playerName, actionType, target, nullptr});
}

void Game::addPendingAction(const std::string& playerName, const std::string& actionType,
                            std::shared_ptr<Player> target, std::shared_ptr<Player> victim) {
    _pendingActions.push_back({playerName, actionType, target, victim});
}

bool Game::hasPendingAction(const std::string& playerName, const std::string& actionType) const {
    for (const auto& act : _pendingActions)
        if (act.playerName == playerName && act.actionType == actionType) return true;
    return false;
}

void Game::clearPendingAction(const std::string& playerName, const std::string& actionType) {
    _pendingActions.erase(
        std::remove_if(_pendingActions.begin(), _pendingActions.end(),
            [&](const auto& a) { return a.playerName == playerName && a.actionType == actionType; }),
        _pendingActions.end());
}

void Game::clearAllPendingActions(const std::string& playerName) {
    _pendingActions.erase(
        std::remove_if(_pendingActions.begin(), _pendingActions.end(),
            [&](const auto& a) { return a.playerName == playerName; }),
        _pendingActions.end());
}

std::vector<Game::PendingAction> Game::getPendingActions() const {
    return _pendingActions;
}

Game::PendingAction Game::getPendingAction(const std::string& playerName, const std::string& actionType) const {
    for (const auto& act : _pendingActions)
        if (act.playerName == playerName && act.actionType == actionType) return act;
    throw GameException("No such pending action");
}

} // namespace coup