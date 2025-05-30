//tomergal40@gmail.com
#include "../include/General.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

General::General(Game& game, const std::string& name)
    : Player(game, name) {}

void General::undo(Player& target) {
    // Check if there's a pending coup on the target
    if (!_game->hasPendingAction(target.getName(), "coup")) {
        throw IllegalMoveException("No coup action to undo!");
    }
    
    // Check if General has enough coins to block a coup
    if (_coins < 5) {
        throw NotEnoughCoinsException("General needs 5 coins to block a coup!");
    }
    
    // Pay 5 coins to bank
    _coins -= 5;
    _game->addToBank(5);
    
    // Reactivate the player if they were eliminated by coup
    target.setActive(true);
    
    // Clear the pending coup action
    _game->clearPendingAction(target.getName(), "coup");
    
    std::cout << "General " << _name << " blocked the coup against " << target.getName() << std::endl;
}

void General::onArrested(Player& by) {
    // General gets the arrested coin back
    if (_coins < 1) {
        return; // No coins to take
    }
    
    _coins -= 1;
    by.addCoins(1);
    addCoins(1); // Refund to self
    
    std::cout << "General " << _name << " got back the coin taken by " << by.getName() << std::endl;
}

void General::prepareCoupDefense(Player& target) {
    // Check if General has enough coins for defense
    if (_coins < 5) {
        throw NotEnoughCoinsException("Not enough coins to prepare coup defense");
    }
    
    // Pay 5 coins to bank
    _coins -= 5;
    _game->addToBank(5);
    
    // Create a pending block_coup action
    _game->addPendingAction(_name, "block_coup", createSafePtr(&target), nullptr);
    
    std::cout << "General " << _name << " prepared to defend " << target.getName() << " against a coup" << std::endl;
}

} // namespace coup