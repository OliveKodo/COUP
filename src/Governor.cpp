//tomergal40@gmail.com
#include "../include/Governor.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Governor::Governor(Game& game, const std::string& name)
    : Player(game, name) {}

void Governor::tax() {
    // Check if it's the Governor's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if Governor is under sanction
    if (_underSanction) {
        throw SanctionedPlayerException("You are under sanction and cannot collect tax!");
    }
    
    // Check if Governor can currently tax
    if (!_canTax) {
        throw IllegalMoveException("You cannot collect tax at this time!");
    }
    
    // Check if Governor must coup (10+ coins)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Special Governor ability: take 3 coins instead of 2
    _game->removeFromBank(3);
    _coins += 3;
    
    // Add a pending action to track this tax collection
    // FIX HERE: Changed *game->addPendingAction(*name, ...) to _game->addPendingAction(_name, ...)
    _game->addPendingAction(_name, "tax", createSafePtr(this), nullptr);
    
    // End the Governor's turn
    _game->nextTurn();
    
    std::cout << "Governor " << _name << " collected 3 coins in tax" << std::endl;
}

void Governor::undo(Player& target) {
    // Check if the target has a pending tax action
    if (_game->hasPendingAction(target.getName(), "tax") && canUndoTax()) {
        // Governor can undo tax actions
        
        // For Governor's tax (3 coins)
        if (target.role() == "Governor") {
            target.removeCoins(3);
            _game->addToBank(3);
        }
        // For regular tax (2 coins)
        else {
            target.removeCoins(2);
            _game->addToBank(2);
        }
        
        // Clear the pending tax action
        _game->clearPendingAction(target.getName(), "tax");
        
        std::cout << "Governor " << _name << " undid the tax collection by " << target.getName() << std::endl;
    }
    // Try the base class implementation for other types of actions
    else {
        Player::undo(target);
    }
}

} // namespace coup