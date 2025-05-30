//tomergal40@gmail.com

#pragma once

#include "Player.hpp"

namespace coup {

class Governor : public Player {
public:
    Governor(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "Governor"; }
    
    // Override methods from the base class
    void tax() override; // Takes 3 coins instead of 2
    
    // Special abilities
    bool canUndoTax() const override { return true; }
    void undo(Player& target) override;
};

} // namespace coup