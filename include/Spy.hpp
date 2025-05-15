#pragma once

#include "Player.hpp"

namespace coup {

class Spy : public Player {
public:
    Spy(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "Spy"; }
    
    // Special abilities - can see another player's coins and block their arrest
    void spyOn(Player& target); // View target's coins
    bool canUndoArrest() const override { return true; }
    void undo(Player& target) override;
};

} // namespace coup