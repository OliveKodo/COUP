//tomergal40@gmail.com
#include "../include/Player.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Player::Player(Game& game, const std::string& name)
    : _name(name), _coins(0), _active(true), _canGather(true), _canTax(true),
      _underSanction(false), _game(&game), _lastArrested(nullptr) {}

std::string Player::getName() const { return _name; }
int Player::coins() const { return _coins; }
bool Player::isActive() const { return _active; }
bool Player::canGather() const { return _canGather; }
bool Player::canTax() const { return _canTax; }
bool Player::isUnderSanction() const { return _underSanction; }
bool Player::mustCoup() const { return _coins >= 10; }

void Player::gather() {
    if (!_game->isPlayerTurn(_name)) throw NotYourTurnException();
    if (_underSanction) throw SanctionedPlayerException("You are under sanction and cannot gather resources!");
    if (!_canGather) throw IllegalMoveException("You cannot gather resources at this time!");
    if (mustCoup()) throw TooManyCoinsException();
    _game->removeFromBank(1);
    _coins += 1;
    _game->addPendingAction(_name, "gather");
    _game->nextTurn();
}

void Player::tax() {
    if (!_game->isPlayerTurn(_name)) throw NotYourTurnException();
    if (_underSanction) throw SanctionedPlayerException("You are under sanction and cannot collect tax!");
    if (!_canTax) throw IllegalMoveException("You cannot collect tax at this time!");
    if (mustCoup()) throw TooManyCoinsException();
    _game->removeFromBank(2);
    _coins += 2;
    _game->addPendingAction(_name, "tax");
    _game->nextTurn();
}

void Player::bribe() {
    if (!_game->isPlayerTurn(_name)) throw NotYourTurnException();
    bool isBaron = (this->role() == "Baron");
    int requiredCoins = isBaron ? 3 : 4;
    if (_coins < requiredCoins)
        throw NotEnoughCoinsException(isBaron ? "Baron requires 3 coins for a bribe!" : "Bribe requires 4 coins!");
    if (mustCoup()) throw TooManyCoinsException();
    _coins -= requiredCoins;
    _game->addToBank(requiredCoins);
    _game->addPendingAction(_name, "bribe");
}

void Player::arrest(Player& target) {
    bool isSpy = (this->role() == "Spy");
    bool isMerchant = (target.role() == "Merchant");
    bool outOfTurnSpyArrest = isSpy && isMerchant;
    if (!outOfTurnSpyArrest && !_game->isPlayerTurn(_name)) throw NotYourTurnException();
    if (mustCoup()) throw TooManyCoinsException();
    if (!target.isActive()) throw PlayerNotActiveException();
    if (&target == _lastArrested) throw IllegalMoveException("Cannot arrest the same player twice in a row!");

    _game->addPendingAction(_name, "arrest", createSafePtr(&target));
    _lastArrested = &target;
    target.onArrested(*this);
    if (!outOfTurnSpyArrest) _game->nextTurn();
}

void Player::sanction(Player& target) {
    if (!_game->isPlayerTurn(_name)) throw NotYourTurnException();
    if (_coins < 3) throw NotEnoughCoinsException("Sanction requires 3 coins!");
    if (mustCoup()) throw TooManyCoinsException();
    if (!target.isActive()) throw PlayerNotActiveException();

    _coins -= 3;
    _game->addToBank(3);
    _game->addPendingAction(_name, "sanction", createSafePtr(&target));
    target.onSanctioned(*this);
    _game->nextTurn();
}

void Player::coup(Player& target) {
    if (!_game->isPlayerTurn(_name)) throw NotYourTurnException();
    if (_coins < 7) throw NotEnoughCoinsException("Coup requires 7 coins!");
    if (!target.isActive()) throw PlayerNotActiveException();

    _coins -= 7;
    _game->addToBank(7);
    _game->addPendingAction(_name, "coup", createSafePtr(this), createSafePtr(&target));
    target.setActive(false);
    _game->nextTurn();
}

void Player::undo(Player& target) {
    if (_game->hasPendingAction(target.getName(), "tax") && canUndoTax()) {
        target.removeCoins(2);
        _game->addToBank(2);
        _game->clearPendingAction(target.getName(), "tax");
    } else if (_game->hasPendingAction(target.getName(), "bribe") && canUndoBribe()) {
        _game->clearPendingAction(target.getName(), "bribe");
    } else {
        throw IllegalMoveException("Cannot undo this action or no action to undo!");
    }
}

void Player::setActive(bool active) { _active = active; }
void Player::setSanction(bool sanctioned) {
    _underSanction = sanctioned;
    _canGather = !sanctioned;
    _canTax = !sanctioned;
}

void Player::addCoins(int amount) { _coins += amount; }
void Player::removeCoins(int amount) {
    if (_coins < amount) throw NotEnoughCoinsException();
    _coins -= amount;
}

void Player::onSanctioned(Player&) { setSanction(true); }

void Player::onArrested(Player& by) {
    if (_coins < 1) return;
    _coins -= 1;
    by.addCoins(1);
}

bool Player::canUndoTax() const { return false; }
bool Player::canUndoBribe() const { return false; }
bool Player::canUndoCoup() const { return false; }
bool Player::canUndoSanction() const { return false; }
bool Player::canUndoArrest() const { return false; }

void Player::startTurn() {
    if (mustCoup()) std::cout << "You have 10+ coins and must perform a coup this turn!" << std::endl;
    if (_underSanction) setSanction(false); 
}

} // namespace coup
