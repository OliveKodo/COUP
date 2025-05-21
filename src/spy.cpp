#include "../include/Spy.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

namespace coup {

Spy::Spy(Game& game, const std::string& name)
    : Player(game, name), lastTargetName("") {}

void Spy::spyOn(Player& target) {
    // Spy ability: reveal target's coin count
    std::cout << "Spy " << _name << " spied on " << target.getName()
              << " and discovered they have " << target.coins() << " coins." << std::endl;
    
    lastTargetName = target.getName();
    
    // Register a pending action to block arrest
    _game->addPendingAction(_name, "block_arrest", createSafePtr(&target), nullptr);
}

void Spy::undo(Player& target) {
    if (target.getName() == lastTargetName) {
        std::cout << "Spy prevents " << target.getName() << " from arresting this turn." << std::endl;
        
        _game->clearPendingAction(_name, "block_arrest");
    } else {
        throw IllegalMoveException("No valid target to undo arrest for.");
    }
}

} // namespace coup