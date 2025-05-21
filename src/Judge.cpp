#include "../include/Judge.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Judge::Judge(Game& game, const std::string& name)
    : Player(game, name) {}

void Judge::undo(Player& target) {
    // Check if the target has a pending bribe action
    if (_game->hasPendingAction(target.getName(), "bribe") && canUndoBribe()) {
        // Judge can undo bribe actions, causing the target to lose the 4 coins they paid
        
        // Clear the pending bribe action
        _game->clearPendingAction(target.getName(), "bribe");
        
        std::cout << "Judge " << _name << " blocked the bribe by " << target.getName() 
                  << ", making them lose the 4 coins they paid" << std::endl;
    } 
    // Try the base class implementation for other types of actions
    else {
        Player::undo(target);
    }
}

void Judge::onSanctioned(Player& by) {
    // First, apply the standard sanction effect
    Player::onSanctioned(by);
    
    // Judge's special ability: when sanctioned, the sanctioner pays an extra coin
    by.removeCoins(1);
    _game->addToBank(1);
    
    std::cout << "Judge " << _name << " was sanctioned by " << by.getName() 
              << ", who had to pay an extra coin as penalty" << std::endl;
}

} // namespace coup