//tomergal40@gmail.com
#pragma once
#include <string>
#include <vector>
#include <memory>

namespace coup {
// Forward declaration
class Player;

class Game {
public:
    // Structure to represent a pending action
    struct PendingAction {
        std::string playerName;
        std::string actionType;
        std::shared_ptr<Player> target;
        std::shared_ptr<Player> victim;
        
        // Constructor with all parameters
        PendingAction(const std::string& name, const std::string& type, 
                      std::shared_ptr<Player> t = nullptr, 
                      std::shared_ptr<Player> v = nullptr)
            : playerName(name), actionType(type), target(t), victim(v) {}
    };
    
private:
    std::vector<std::shared_ptr<Player>> _players;
    int _currentTurn;
    int _bank;
    bool _gameStarted;
    std::vector<PendingAction> _pendingActions;
    
public:
    Game();
    
    // Player management
    void addPlayer(std::shared_ptr<Player> player);
    void removePlayer(const std::string& name);
    std::shared_ptr<Player> getPlayer(const std::string& name);
    std::shared_ptr<Player> getCurrentPlayer();
    
    // Game state and turn management
    std::string turn() const;
    std::vector<std::string> players() const;
    std::string winner() const;
    void startGame();
    bool isGameOver() const;
    void nextTurn();
    
    // Bank operations
    int getBank() const;
    void removeFromBank(int amount);
    void addToBank(int amount);
    
    // Turn checking
    bool isPlayerTurn(const std::string& playerName) const;
    int countActivePlayers() const;
    
    // Pending actions management
    void addPendingAction(const std::string& playerName, const std::string& actionType);
    void addPendingAction(const std::string& playerName, const std::string& actionType, 
                         std::shared_ptr<Player> target);
    void addPendingAction(const std::string& playerName, const std::string& actionType, 
                         std::shared_ptr<Player> target, std::shared_ptr<Player> victim);
    bool hasPendingAction(const std::string& playerName, const std::string& actionType) const;
    void clearPendingAction(const std::string& playerName, const std::string& actionType);
    void clearAllPendingActions(const std::string& playerName);
    std::vector<PendingAction> getPendingActions() const;
    PendingAction getPendingAction(const std::string& playerName, const std::string& actionType) const;
};

} // namespace coup