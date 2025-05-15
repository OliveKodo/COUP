#include "../include/General.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"

namespace coup {

General::General(Game& game, const std::string& name) 
    : Player(game, name) {
}

void General::undo(Player& target) {
    // Check if there's a coup to undo
    if (_game->hasPendingAction(target.getName(), "coup") && canUndoCoup()) {
        // Check if general has enough coins
        if (_coins < 5) {
            throw NotEnoughCoinsException("Blocking a coup requires 5 coins!");
        }
        
        // Get the victim
        auto action = _game->getPendingAction(target.getName(), "coup");
        Player* victim = action.victim.get();
        
        // Pay 5 coins to block the coup
        _coins -= 5;
        _game->addToKupah(5);
        
        // Restore the victim
        if (victim) {
            victim->setActive(true);
        }
        
        _game->clearPendingAction(target.getName(), "coup");
    } else {
        // Call base class undo for other actions
        Player::undo(target);
    }
}

void General::onArrested(Player& by) {
    // General gets the arrested coin back
    // First, execute standard arrest behavior
    Player::onArrested(by);
    
    // Then, take the coin back
    by.removeCoins(1);
    _coins += 1;
}

} // namespace coup