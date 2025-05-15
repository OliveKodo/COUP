
#include "../include/Player.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Player::Player(Game& game, const std::string& name) 
    : _name(name), 
      _coins(0), 
      _active(true), 
      _canGather(true), 
      _canTax(true), 
      _underSanction(false), 
      _game(&game), 
      _lastArrested(nullptr) {
}

std::string Player::getName() const {
    return _name;
}

int Player::coins() const {
    return _coins;
}

bool Player::isActive() const {
    return _active;
}

bool Player::canGather() const {
    return _canGather;
}

bool Player::canTax() const {
    return _canTax;
}

bool Player::isUnderSanction() const {
    return _underSanction;
}

bool Player::mustCoup() const {
    return _coins >= 10;
}

void Player::gather() {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player is under sanction
    if (_underSanction) {
        throw SanctionedPlayerException("You are under sanction and cannot gather resources!");
    }
    
    // Check if player can gather (not blocked)
    if (!_canGather) {
        throw IllegalMoveException("You cannot gather resources at this time!");
    }
    
    // Check if player has 10+ coins (must coup)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Gather 1 coin
    _game->removeFromKupah(1);
    _coins += 1;
    
    // Register this action as pending (can be blocked)
    _game->addPendingAction(_name, "gather");
    
    // Move to next player's turn
    _game->nextTurn();
}

void Player::tax() {
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
    
    // Collect 2 coins as tax
    _game->removeFromKupah(2);
    _coins += 2;
    
    // Register this action as pending (can be blocked)
    _game->addPendingAction(_name, "tax");
    
    // Move to next player's turn
    _game->nextTurn();
}

void Player::bribe() {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player has enough coins
    if (_coins < 4) {
        throw NotEnoughCoinsException("Bribe requires 4 coins!");
    }
    
    // Check if player has 10+ coins (must coup)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Pay 4 coins
    _coins -= 4;
    _game->addToKupah(4);
    
    // Register this action as pending (can be blocked by Judge)
    _game->addPendingAction(_name, "bribe");
    
    // Note: Do not call nextTurn() as bribe gives an additional action
}

void Player::arrest(Player& target) {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player has 10+ coins (must coup)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Check if target is active
    if (!target.isActive()) {
        throw PlayerNotActiveException();
    }
    
    // Check if target is the same as last arrested
    if (&target == _lastArrested) {
        throw IllegalMoveException("Cannot arrest the same player twice in a row!");
    }
    
    // Register this action as pending (can be blocked)
    _game->addPendingAction(_name, "arrest", std::shared_ptr<Player>(&target, [](Player*){}));
    
    // Perform arrest
    _lastArrested = &target;
    target.onArrested(*this);
    
    // Move to next player's turn
    _game->nextTurn();
}

void Player::sanction(Player& target) {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player has enough coins
    if (_coins < 3) {
        throw NotEnoughCoinsException("Sanction requires 3 coins!");
    }
    
    // Check if player has 10+ coins (must coup)
    if (mustCoup()) {
        throw TooManyCoinsException();
    }
    
    // Check if target is active
    if (!target.isActive()) {
        throw PlayerNotActiveException();
    }
    
    // Pay 3 coins
    _coins -= 3;
    _game->addToKupah(3);
    
    // Register this action as pending (can be blocked)
    _game->addPendingAction(_name, "sanction", std::shared_ptr<Player>(&target, [](Player*){}));
    
    // Apply sanction
    target.onSanctioned(*this);
    
    // Move to next player's turn
    _game->nextTurn();
}

void Player::coup(Player& target) {
    // Check if it's player's turn
    if (!_game->isPlayerTurn(_name)) {
        throw NotYourTurnException();
    }
    
    // Check if player has enough coins
    if (_coins < 7) {
        throw NotEnoughCoinsException("Coup requires 7 coins!");
    }
    
    // Check if target is active
    if (!target.isActive()) {
        throw PlayerNotActiveException();
    }
    
    // Pay 7 coins
    _coins -= 7;
    _game->addToKupah(7);
    
    // Register this action as pending (can be blocked by General)
    _game->addPendingAction(_name, "coup", std::shared_ptr<Player>(this, [](Player*){})
                           , std::shared_ptr<Player>(&target, [](Player*){}));
    
    // Remove target from game if not blocked
    target.setActive(false);
    
    // Move to next player's turn
    _game->nextTurn();
}

void Player::undo(Player& target) {
    // Check which action to undo based on pending actions
    if (_game->hasPendingAction(target.getName(), "tax") && canUndoTax()) {
        // Undo tax
        target.removeCoins(2); // Standard tax is 2 coins
        _game->addToKupah(2);
        _game->clearPendingAction(target.getName(), "tax");
    } else if (_game->hasPendingAction(target.getName(), "bribe") && canUndoBribe()) {
        // Undo bribe
        // Implementation depends on the exact rules
        _game->clearPendingAction(target.getName(), "bribe");
    } else {
        throw IllegalMoveException("Cannot undo this action or no action to undo!");
    }
}

void Player::setActive(bool active) {
    _active = active;
}

void Player::setSanction(bool sanctioned) {
    _underSanction = sanctioned;
    
    // If sanctioned, player can't gather or tax
    if (sanctioned) {
        _canGather = false;
        _canTax = false;
    } else {
        _canGather = true;
        _canTax = true;
    }
}

void Player::addCoins(int amount) {
    _coins += amount;
}

void Player::removeCoins(int amount) {
    if (_coins < amount) {
        throw NotEnoughCoinsException();
    }
    _coins -= amount;
}

void Player::onSanctioned(Player&) {
    // Default behavior: set the sanction flag
    setSanction(true);
}

void Player::onArrested(Player& by) {
    // Default behavior: lose 1 coin to the arrester
    if (_coins < 1) {
        // If no coins, nothing happens
        return;
    }
    
    _coins -= 1;
    by.addCoins(1);
}

bool Player::canUndoTax() const {
    return false; // Base player can't undo tax
}

bool Player::canUndoBribe() const {
    return false; // Base player can't undo bribe
}

bool Player::canUndoCoup() const {
    return false; // Base player can't undo coup
}

bool Player::canUndoSanction() const {
    return false; // Base player can't undo sanction
}

bool Player::canUndoArrest() const {
    return false; // Base player can't undo arrest
}

void Player::startTurn() {
    // Check for 10+ coins at the start of turn
    if (mustCoup()) {
        std::cout << "You have 10+ coins and must perform a coup this turn!" << std::endl;
    }
    
    // Clear sanctions at the start of turn
    if (_underSanction) {
        setSanction(false);
    }
    
    // Base player has no special start-of-turn effects
    // This will be overridden in classes like Merchant
}

} // namespace coup