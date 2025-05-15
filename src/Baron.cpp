#include "../include/Baron.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"

namespace coup {

Baron::Baron(Game& game, const std::string& name) 
    : Player(game, name) {
}

void Baron::invest() {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player has enough coins
    if (_coins < 3) {
        throw NotEnoughCoinsException("Investment requires 3 coins!");
    }
    
    // Check if player has 10+ coins (must coup)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Pay 3 coins and get 6 in return
    _coins -= 3;
    _game->addToKupah(3);
    _coins += 6;
    
    // Register this action as pending
    _game->addPendingAction(_name, "invest");
    
    // Move to next player's turn
    _game->nextTurn();
}

void Baron::onSanctioned(Player& by) {
    // When Baron is sanctioned, he gets 1 coin as compensation
    Player::onSanctioned(by);
    _coins += 1;
}

} // namespace coup