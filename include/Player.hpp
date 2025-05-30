//tomergal40@gmail.com
#pragma once
#include <string>
#include <memory>

namespace coup {
// Forward declaration to prevent circular dependency
class Game;

class Player {
protected:
    std::string _name;
    int _coins;
    bool _active;
    bool _canGather;
    bool _canTax;
    bool _underSanction;
    Game* _game;
    Player* _lastArrested; // The last player arrested by this player
    
    // Helper method to create non-owning shared pointers
    static std::shared_ptr<Player> createSafePtr(Player* ptr) {
        return std::shared_ptr<Player>(ptr, [](Player*){/* empty deleter */});
    }
    
public:
    Player(Game& game, const std::string& name);
    virtual ~Player() = default;
    
    // Role identification
    virtual std::string role() const { return "Player"; }
    
    // getters
    std::string getName() const;
    int coins() const; // Changed from getCoins() to coins()
    bool isActive() const;
    bool canGather() const;
    bool canTax() const;
    bool isUnderSanction() const;
    
    // Basic actions
    virtual void gather();
    virtual void tax();
    virtual void bribe();
    virtual void arrest(Player& target);
    virtual void sanction(Player& target);
    virtual void coup(Player& target);
    
    // Undo actions (for blocking)
    virtual void undo(Player& target);
    
    // Internal methods to manage player state
    void setActive(bool active);
    void setSanction(bool sanctioned);
    void addCoins(int amount);
    void removeCoins(int amount);
    
    // Reaction methods to other players' actions
    virtual void onSanctioned(Player& by);
    virtual void onArrested(Player& by);
    
    // Check if player can undo different types of actions
    virtual bool canUndoTax() const;
    virtual bool canUndoBribe() const;
    virtual bool canUndoCoup() const;
    virtual bool canUndoSanction() const;
    virtual bool canUndoArrest() const;
    
    // Additional virtual methods as needed
    virtual void startTurn(); // Actions that occur at the beginning of a turn
    
    // Helper method to check if player must coup
    bool mustCoup() const;
};

} // namespace coup