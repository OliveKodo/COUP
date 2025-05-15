#pragma once

#include "Player.hpp"

namespace coup {

class Baron : public Player {
public:
    Baron(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "Baron"; }
    
    // Special abilities
    void invest(); // Invest 3 coins to get 6 in return
    void onSanctioned(Player& by) override; // Get 1 coin compensation when sanctioned
};

} // namespace coup