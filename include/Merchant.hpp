#pragma once

#include "Player.hpp"

namespace coup {

class Merchant : public Player {
public:
    Merchant(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "Merchant"; }
    
    // Special abilities
    void startTurn() override; // Get an extra coin if starting turn with 3+ coins
    void onArrested(Player& by) override; // Pay 2 coins to treasury instead of losing 1 to arrester
};

} // namespace coup