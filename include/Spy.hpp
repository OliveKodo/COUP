//tomergal40@gmail.com
#pragma once
#include "Player.hpp"
#include <string>

namespace coup {
class Spy : public Player {
private:
    std::string lastTargetName;

public:
    Spy(Game& game, const std::string& name);
    
    // Role identification
    std::string role() const override { return "Spy"; }
    
    // Special abilities
    void spyOn(Player& target);
    void undo(Player& target) override;
    bool canUndoArrest() const override { return true; }
};
} // namespace coup