#pragma once

#include "Player.hpp"

namespace coup {

class Judge : public Player {
public:
    Judge(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "Judge"; }
    
    // Special abilities
    bool canUndoBribe() const override { return true; }
    void undo(Player& target) override; // Block bribe, making target lose 4 coins
    void onSanctioned(Player& by) override; // When sanctioned, the sanctioner pays an extra coin
};

} // namespace coup