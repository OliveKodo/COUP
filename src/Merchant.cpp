//tomergal40@gmail.com
#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Merchant::Merchant(Game& game, const std::string& name)
    : Player(game, name) {}

void Merchant::startTurn() {
    // First, apply the standard turn start behavior
    Player::startTurn();
    
    // Merchant's special ability: get an extra coin if starting turn with 3+ coins
    if (_coins >= 3) {
        _coins += 1;
        _game->removeFromBank(1);
        
        std::cout << "Merchant " << _name << " received an extra coin at the start of their turn" << std::endl;
    }
}

void Merchant::onArrested(Player& by) {
    // Merchant's special ability: pay 2 coins to treasury instead of losing 1 to arrester
    
    // Check if Merchant has enough coins
    if (_coins >= 2) {
        // Pay 2 coins to bank instead of giving 1 to the arrester
        _coins -= 2;
        _game->addToBank(2);
        
        std::cout << "Merchant " << _name << " paid 2 coins to the treasury when arrested by " 
                  << by.getName() << std::endl;
    } 
    // If not enough coins, nothing happens (merchant is broke)
    else {
        std::cout << "Merchant " << _name << " doesn't have enough coins to pay the arrest fee" << std::endl;
    }
    
    // Note: We're intentionally not calling Player::onArrested() since the behavior is completely different
}

} // namespace coup