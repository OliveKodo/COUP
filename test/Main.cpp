// Main.cpp
//tomergal40@gmail.com

#include "../include/Player.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>

using namespace std;
using namespace coup;

// Displays a summary of the game state
void displayGameState(const Game& game) {
    cout << "\n" << string(50, '=') << endl;
    cout << "Current Game State:" << endl;
    cout << "Active Players: ";
    for (const auto& name : game.players()) {
        cout << name << " ";
    }
    cout << "\nCurrent Turn: " << game.turn() << endl;
    cout << string(50, '=') << "\n" << endl;
}

// Displays single player's name, role, and coin count
void displayPlayerInfo(const shared_ptr<Player>& player) {
    cout << player->getName() << " (" << player->role() << ") - " 
         << player->coins() << " coins" << endl;
}

// Displays status for all players
void displayAllPlayers(const vector<shared_ptr<Player>>& players) {
    cout << "\nPlayer Status:" << endl;
    for (const auto& player : players) {
        if (player->isActive()) {
            displayPlayerInfo(player);
        } else {
            cout << player->getName() << " (ELIMINATED)" << endl;
        }
    }
    cout << endl;
}

int main() {
    try {
        cout << "=== COUP GAME DEMONSTRATION ===" << endl;

        Game game;

        // Create players with roles
        shared_ptr<Governor> governor = make_shared<Governor>(game, "Moshe");
        shared_ptr<Spy> spy = make_shared<Spy>(game, "Yossi");
        shared_ptr<Baron> baron = make_shared<Baron>(game, "Meirav");
        shared_ptr<General> general = make_shared<General>(game, "Reut");
        shared_ptr<Judge> judge = make_shared<Judge>(game, "Gilad");
        shared_ptr<Merchant> merchant = make_shared<Merchant>(game, "Sarah");

        vector<shared_ptr<Player>> allPlayers = {governor, spy, baron, general, judge, merchant};

        for (const auto& player : allPlayers) {
            game.addPlayer(player);
            cout << "Added player: " << player->getName() << " as " << player->role() << endl;
        }

        game.startGame();
        displayGameState(game);

        // ROUND 1
        cout << "=== ROUND 1: Basic Actions ===" << endl;
        
        // Ensure it's Moshe's turn and he gathers
        while (game.turn() != governor->getName()) game.nextTurn();
        governor->gather();
        cout << "Moshe gathered 1 coin" << endl;
        
        // Yossi's turn
        while (game.turn() != spy->getName()) game.nextTurn();
        spy->tax();
        cout << "Yossi used tax ability" << endl;
        
        // Meirav's turn
        while (game.turn() != baron->getName()) game.nextTurn();
        baron->gather();
        cout << "Meirav gathered 1 coin" << endl;
        
        // Reut's turn
        while (game.turn() != general->getName()) game.nextTurn();
        general->tax();
        cout << "Reut used tax ability" << endl;
        
        // Gilad's turn
        while (game.turn() != judge->getName()) game.nextTurn();
        judge->gather();
        cout << "Gilad gathered 1 coin" << endl;
        
        // Sarah's turn
        while (game.turn() != merchant->getName()) game.nextTurn();
        merchant->gather();
        cout << "Sarah gathered 1 coin" << endl;
        
        displayAllPlayers(allPlayers);

        // ROUND 2
        cout << "=== ROUND 2: Special Abilities ===" << endl;
        
        // Moshe's turn
        while (game.turn() != governor->getName()) game.nextTurn();
        governor->tax();
        cout << "Moshe used Governor tax ability (3 coins)" << endl;
        
        // Yossi's turn
        while (game.turn() != spy->getName()) game.nextTurn();
        spy->spyOn(*baron);
        cout << "Yossi (Spy) spied on Meirav - she has " << baron->coins() << " coins" << endl;
        
        // Meirav's turn
        while (game.turn() != baron->getName()) game.nextTurn();
        baron->gather();
        cout << "Meirav gathered 1 coin" << endl;
        
        // Meirav's second action (if allowed)
        if (game.turn() == baron->getName()) {
            baron->invest();
            cout << "Meirav used invest ability" << endl;
        }
        
        // Reut's turn
        while (game.turn() != general->getName()) game.nextTurn();
        general->gather();
        cout << "Reut gathered 1 coin" << endl;
        
        // Gilad's turn
        while (game.turn() != judge->getName()) game.nextTurn();
        judge->gather();
        cout << "Gilad gathered 1 coin" << endl;
        
        // Sarah's turn
        while (game.turn() != merchant->getName()) game.nextTurn();
        merchant->gather();
        cout << "Sarah gathered 1 coin" << endl;
        
        displayAllPlayers(allPlayers);

        // ROUND 3
        cout << "=== ROUND 3: Interactions ===" << endl;
        
        // Moshe's turn
        while (game.turn() != governor->getName()) game.nextTurn();
        governor->arrest(*spy);
        cout << "Moshe arrested Yossi" << endl;
        
        // Yossi's turn (if not arrested or can still act)
        while (game.turn() != spy->getName()) game.nextTurn();
        try {
            if (spy->coins() >= 3) {
                spy->sanction(*baron);
                cout << "Yossi sanctioned Meirav" << endl;
            } else {
                spy->gather();
                cout << "Yossi gathered coins" << endl;
            }
        } catch (const exception& e) {
            cout << "Yossi couldn't act: " << e.what() << endl;
        }
        
        // Meirav's turn
        while (game.turn() != baron->getName()) game.nextTurn();
        try {
            baron->gather();
            cout << "Meirav gathered coins" << endl;
        } catch (const exception& e) {
            cout << "Meirav couldn't act: " << e.what() << endl;
        }
        
        displayAllPlayers(allPlayers);

        // ROUND 4
        cout << "=== ROUND 4: Building Wealth ===" << endl;
        
        // Let each player take several turns to build up coins
        for (int round = 0; round < 3; round++) {
            cout << "\n--- Sub-round " << (round + 1) << " ---" << endl;
            
            // Moshe's turn
            while (game.turn() != governor->getName()) game.nextTurn();
            try {
                if (governor->coins() >= 4 && round == 2) {
                    governor->bribe();
                    cout << "Moshe used bribe" << endl;
                } else {
                    governor->gather();
                    cout << "Moshe gathered coins" << endl;
                }
            } catch (const exception& e) {
                cout << "Moshe couldn't act: " << e.what() << endl;
            }
            
            // Let other active players take turns
            for (auto& player : allPlayers) {
                if (player != governor && player->isActive()) {
                    while (game.turn() != player->getName()) game.nextTurn();
                    try {
                        player->gather();
                        cout << player->getName() << " gathered coins" << endl;
                    } catch (const exception& e) {
                        cout << player->getName() << " couldn't act: " << e.what() << endl;
                    }
                }
            }
        }
        
        displayAllPlayers(allPlayers);

        // ROUND 5: SANCTIONS DEMONSTRATION
        cout << "=== ROUND 5: Sanctions and Effects ===" << endl;
        
        // Someone sanctions another player
        while (game.turn() != baron->getName()) game.nextTurn();
        try {
            baron->sanction(*judge);
            cout << "Meirav (Baron) sanctioned Gilad (Judge)" << endl;
        } catch (const exception& e) {
            cout << "Sanction attempt: " << e.what() << endl;
        }
        
        // Try to act while sanctioned
        while (game.turn() != judge->getName()) game.nextTurn();
        try {
            judge->gather();
            cout << "Gilad gathered despite sanction" << endl;
        } catch (const exception& e) {
            cout << "Sanction effect: " << e.what() << endl;
        }
        
        displayAllPlayers(allPlayers);

        // ROUND 6: BLOCKING MECHANICS
        cout << "=== ROUND 6: Blocking Mechanics ===" << endl;
        
        // Show blocking attempt
        while (game.turn() != spy->getName()) game.nextTurn();
        try {
            spy->tax();
            cout << "Yossi used tax successfully" << endl;
        } catch (const exception& e) {
            cout << "Tax blocked: " << e.what() << endl;
        }
        
        // Try to undo someone's action
        while (game.turn() != general->getName()) game.nextTurn();
        try {
            general->undo(*spy);
            cout << "Reut attempted to undo Yossi's action" << endl;
        } catch (const exception& e) {
            cout << "Undo attempt: " << e.what() << endl;
        }
        
        displayAllPlayers(allPlayers);

        // ROUND 7: BUILDING UP FOR COUPS
        cout << "=== ROUND 7: Building for Coups ===" << endl;
        
        // Give players more coins to enable coups
        for (int buildRound = 0; buildRound < 2; buildRound++) {
            cout << "\n--- Building round " << (buildRound + 1) << " ---" << endl;
            
            for (auto& player : allPlayers) {
                if (player->isActive()) {
                    while (game.turn() != player->getName()) game.nextTurn();
                    try {
                        if (player->coins() >= 3) {
                            player->tax();
                            cout << player->getName() << " used tax (" << player->coins() << " coins)" << endl;
                        } else {
                            player->gather();
                            cout << player->getName() << " gathered (" << player->coins() << " coins)" << endl;
                        }
                    } catch (const exception& e) {
                        cout << player->getName() << " couldn't act: " << e.what() << endl;
                    }
                }
            }
        }
        
        displayAllPlayers(allPlayers);

        // ROUND 8: COUP ATTEMPTS AND ELIMINATIONS
        cout << "=== ROUND 8: Coup Attempts! ===" << endl;
        
        // Look for someone with enough coins to coup
        bool coupAttempted = false;
        for (auto& attacker : allPlayers) {
            if (!coupAttempted && attacker->isActive() && attacker->coins() >= 7) {
                while (game.turn() != attacker->getName()) game.nextTurn();
                
                // Find a target (first active player that isn't the attacker)
                for (auto& target : allPlayers) {
                    if (target->isActive() && target != attacker) {
                        try {
                            cout << "💥 " << attacker->getName() << " (" << attacker->coins() 
                                 << " coins) attempts to coup " << target->getName() << "!" << endl;
                            attacker->coup(*target);
                            cout << "✅ Coup successful! " << target->getName() << " is eliminated!" << endl;
                            coupAttempted = true;
                            break;
                        } catch (const exception& e) {
                            cout << "❌ Coup failed: " << e.what() << endl;
                        }
                    }
                }
                if (coupAttempted) break;
            }
        }
        
        if (!coupAttempted) {
            cout << "No one has enough coins for a coup yet." << endl;
        }
        
        displayAllPlayers(allPlayers);

        // ROUND 9: FINAL ROUNDS
        cout << "=== ROUND 9: Final Push ===" << endl;
        
        // Continue for a few more rounds to see eliminations
        for (int finalRound = 0; finalRound < 5; finalRound++) {
            cout << "\n--- Final round " << (finalRound + 1) << " ---" << endl;
            
            // Count active players
            int activePlayers = 0;
            for (auto& player : allPlayers) {
                if (player->isActive()) activePlayers++;
            }
            
            if (activePlayers <= 1) {
                cout << "Game over - only one player remains!" << endl;
                break;
            }
            
            // Each active player takes a turn
            for (auto& player : allPlayers) {
                if (player->isActive()) {
                    while (game.turn() != player->getName()) game.nextTurn();
                    
                    try {
                        // If they have enough coins, try to coup
                        if (player->coins() >= 7) {
                            // Find someone to coup
                            for (auto& target : allPlayers) {
                                if (target->isActive() && target != player) {
                                    cout << "💥 " << player->getName() << " coups " << target->getName() << "!" << endl;
                                    player->coup(*target);
                                    cout << "☠️ " << target->getName() << " eliminated!" << endl;
                                    goto next_round; // Break out of nested loops
                                }
                            }
                        } else {
                            // Otherwise gather more coins
                            player->gather();
                            cout << player->getName() << " gathers (" << player->coins() << " coins)" << endl;
                        }
                    } catch (const exception& e) {
                        cout << player->getName() << " couldn't act: " << e.what() << endl;
                    }
                }
            }
            next_round:;
        }
        
        displayAllPlayers(allPlayers);

        // ERROR HANDLING TESTS
        cout << "\n=== ERROR HANDLING TESTS ===" << endl;
        
        // Test 1: Invalid turn
        try {
            // Find someone whose turn it isn't
            for (auto& player : allPlayers) {
                if (player->isActive() && game.turn() != player->getName()) {
                    player->gather();
                    break;
                }
            }
        } catch (const exception& e) {
            cout << "Invalid turn error: " << e.what() << endl;
        }

        // Test 2: Self-targeting
        try {
            governor->arrest(*governor);
        } catch (const exception& e) {
            cout << "Self-target error: " << e.what() << endl;
        }

        // FINAL WINNER CHECK
        cout << "\n=== FINAL GAME RESULTS ===" << endl;
        
        // Count final active players
        vector<shared_ptr<Player>> survivors;
        for (auto& player : allPlayers) {
            if (player->isActive()) {
                survivors.push_back(player);
            }
        }
        
        if (survivors.size() == 1) {
            cout << "🏆 WINNER: " << survivors[0]->getName() 
                 << " (" << survivors[0]->role() << ") with " 
                 << survivors[0]->coins() << " coins!" << endl;
        } else if (survivors.size() > 1) {
            cout << "🤝 Multiple survivors:" << endl;
            for (auto& survivor : survivors) {
                cout << "  - " << survivor->getName() << " (" << survivor->role() 
                     << ") with " << survivor->coins() << " coins" << endl;
            }
        } else {
            cout << "🤔 No survivors? Something went wrong!" << endl;
        }

        cout << "\n=== END OF DEMO ===" << endl;

    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}