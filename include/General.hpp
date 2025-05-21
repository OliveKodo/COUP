#pragma once

#include "Player.hpp"

namespace coup {

class General : public Player {
public:
    General(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "General"; }
    
    // Special abilities
    bool canUndoCoup() const override { return true; }
    void undo(Player& target) override; // Pay 5 coins to block a coup
    void onArrested(Player& by) override; // Get the arrested coin back
    void prepareCoupDefense(Player& target);

};

} // namespace coup