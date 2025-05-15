#include "../include/Judge.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"

namespace coup {

Judge::Judge(Game& game, const std::string& name) 
    : Player(game, name) {
}

void Judge::undo(Player& target) {
    // Check if there's a bribe to undo
    if (_game->hasPendingAction(target.getName(), "bribe") && canUndoBribe()) {
        // The target loses their 4 coins (they were spent on the bribe)
        // No need to make them pay again, as they already paid in the bribe action
        
        _game->clearPendingAction(target.getName(), "bribe");
    } else {
        // Call base class undo for other actions
        Player::undo(target);
    }
}

void Judge::onSanctioned(Player& by) {
    // First, execute standard sanction behavior
    Player::onSanctioned(by);
    
    // Then, make the sanctioner pay an extra coin
    by.removeCoins(1);
    _game->addToKupah(1);
}

} // namespace coup