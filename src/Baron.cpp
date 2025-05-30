#include "../include/Baron.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>
//tomergal40@gmail.com

namespace coup {

Baron::Baron(Game& game, const std::string& name)
    : Player(game, name) {}

void Baron::invest() {
    // Check if it's the Baron's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if Baron has enough coins to invest
    if (_coins < 3) {
        throw NotEnoughCoinsException("Baron needs 3 coins to invest!");
    }
    
    // Check if Baron must coup (10+ coins)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Pay 3 coins for the investment
    _coins -= 3;
    _game->addToBank(3);
    
    // Get 6 coins in return (profit of 3 coins)
    _game->removeFromBank(6);
    _coins += 6;
    
    // Add a pending action to track this investment
    _game->addPendingAction(_name, "invest", createSafePtr(this), nullptr);
    
    // End the Baron's turn
    _game->nextTurn();
    
    std::cout << "Baron " << _name << " invested 3 coins and received 6 in return!" << std::endl;
}

void Baron::onSanctioned(Player& by) {
    // Base sanction behavior
    Player::onSanctioned(by);
    
    // Baron's special ability: get 1 coin compensation when sanctioned
    _coins += 1;
    _game->removeFromBank(1);
    _game->addPendingAction(_name, "compensation", createSafePtr(this), nullptr);
    
    
    std::cout << "Baron " << _name << " received 1 coin compensation after being sanctioned by "
              << by.getName() << "!" << std::endl;
}

} // namespace coup