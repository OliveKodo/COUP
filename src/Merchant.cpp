#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"

namespace coup {

Merchant::Merchant(Game& game, const std::string& name) 
    : Player(game, name) {
}

void Merchant::startTurn() {
    // First, handle standard start of turn effects
    Player::startTurn();
    
    // If merchant has 3+ coins, get an extra coin
    if (_coins >= 3) {
        _coins += 1;
    }
}

void Merchant::onArrested(Player&) {
    // Merchant pays 2 coins to treasury instead of 1 to arrester
    if (_coins < 2) {
        // If not enough coins, just pay what you have
        _game->addToKupah(_coins);
        _coins = 0;
    } else {
        _coins -= 2;
        _game->addToKupah(2);
    }
    
    // Note: We don't call Player::onArrested() here because we're replacing the behavior
}

} // namespace coup