#include "../include/Spy.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Spy::Spy(Game& game, const std::string& name) 
    : Player(game, name) {
}

void Spy::spyOn(Player& target) {
    // This doesn't count as an action, so no turn checks
    // Simply print out the target's coins (in a real implementation, you might want to return this value)
    std::cout << target.getName() << " has " << target.coins() << " coins." << std::endl;
    
    // Prevent target from arresting in their next turn
    _game->addPendingAction(target.getName(), "spied_on");
}

void Spy::undo(Player& target) {
    // Check if there's an arrest to undo
    if (_game->hasPendingAction(target.getName(), "arrest") && canUndoArrest()) {
        // Get the arrested player
        auto action = _game->getPendingAction(target.getName(), "arrest");
        Player* arrestedPlayer = action.target.get();
        
        // Undo the arrest (return the coin)
        if (arrestedPlayer) {
            target.removeCoins(1);
            arrestedPlayer->addCoins(1);
        }
        
        _game->clearPendingAction(target.getName(), "arrest");
    } else {
        // Call base class undo for other actions
        Player::undo(target);
    }
}

} // namespace coup