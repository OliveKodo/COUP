#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdlib> // For random role assignment
#include <ctime>   // For random seed
#include "Player.hpp"

namespace coup {

// Forward declarations to avoid circular dependencies
class Governor;
class Spy;
class Baron;
class General;
class Judge;
class Merchant;

class Game {
private:
    std::vector<std::shared_ptr<Player>> _players;
    int _currentTurn;
    int _kupah; // The central treasury of coins
    bool _gameStarted;
    
    // For handling "real-time" blocking actions
    struct PendingAction {
        std::string playerName;
        std::string actionType; // "tax", "bribe", "coup", etc.
        std::shared_ptr<Player> target; // For actions with targets
        std::shared_ptr<Player> victim; // For coup blocking
    };
    
    std::vector<PendingAction> _pendingActions;
    
public:
    Game();
    
    // Player management
    void addPlayer(std::shared_ptr<Player> player);
    std::shared_ptr<Player> addPlayerWithRandomRole(const std::string& name);
    void removePlayer(const std::string& name);
    std::shared_ptr<Player> getPlayer(const std::string& name);
    std::shared_ptr<Player> getCurrentPlayer();
    
    // Required methods from the assignment
    std::string turn() const;
    std::vector<std::string> players() const;
    std::string winner() const;
    
    // Game management
    void startGame();
    bool isGameOver() const;
    void nextTurn();
    
    // Treasury management
    int getKupah() const;
    void removeFromKupah(int amount);
    void addToKupah(int amount);
    
    // Helper methods
    bool isPlayerTurn(const std::string& playerName) const;
    int countActivePlayers() const;
    
    // Methods to manage pending actions for "real-time" blocking
    void addPendingAction(const std::string& playerName, const std::string& actionType, 
                          std::shared_ptr<Player> target = nullptr,
                          std::shared_ptr<Player> victim = nullptr);
    bool hasPendingAction(const std::string& playerName, const std::string& actionType) const;
    void clearPendingAction(const std::string& playerName, const std::string& actionType);
    void clearAllPendingActions(const std::string& playerName);
    std::vector<PendingAction> getPendingActions() const;
    PendingAction getPendingAction(const std::string& playerName, const std::string& actionType) const;
};

} // namespace coup