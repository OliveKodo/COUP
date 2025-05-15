#include "../include/Governor.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"

namespace coup {

Governor::Governor(Game& game, const std::string& name) 
    : Player(game, name) {
}

void Governor::tax() {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player is under sanction
    if (_underSanction) {
        throw SanctionedPlayerException("You are under sanction and cannot collect tax!");
    }
    
    // Check if player can tax (not blocked)
    if (!_canTax) {
        throw IllegalMoveException("You cannot collect tax at this time!");
    }
    
    // Check if player has 10+ coins (must coup)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Governor collects 3 coins as tax instead of 2
    _game->removeFromKupah(3);
    _coins += 3;
    
    // Register this action as pending (can be blocked)
    _game->addPendingAction(_name, "tax");
    
    // Move to next player's turn
    _game->nextTurn();
}

void Governor::undo(Player& target) {
    // Check which action to undo
    if (_game->hasPendingAction(target.getName(), "tax")) {
        // Determine how many coins to take back based on target's role
        int taxAmount = 2; // Default tax amount
        
        if (target.role() == "Governor") {
            taxAmount = 3; // Governor tax amount
        }
        
        // Take the tax back
        target.removeCoins(taxAmount);
        _game->addToKupah(taxAmount);
        _game->clearPendingAction(target.getName(), "tax");
    } else {
        // Call base class undo for other actions
        Player::undo(target);
    }
}

} // namespace coup