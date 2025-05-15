// Your name and email here

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
#include <stdexcept>
#include <vector>
#include <memory>

using namespace std;
using namespace coup;

int main() {
    Game game_1{};
    
    // Create shared_ptr objects for the different players
    auto governor = make_shared<Governor>(game_1, "Moshe");
    auto spy = make_shared<Spy>(game_1, "Yossi");
    auto baron = make_shared<Baron>(game_1, "Meirav");
    auto general = make_shared<General>(game_1, "Reut");
    auto judge = make_shared<Judge>(game_1, "Gilad");
    
    // Add players to the game - critical part!
    game_1.addPlayer(governor);
    game_1.addPlayer(spy);
    game_1.addPlayer(baron);
    game_1.addPlayer(general);
    game_1.addPlayer(judge);
    
    // Print the list of active players
    cout << "Active players:" << endl;
    vector<string> players = game_1.players();
    for(string name : players){
        cout << name << endl;
    }
    
    // Show whose turn it is
    cout << "\nCurrent turn: " << game_1.turn() << endl;
    
    // Demo gameplay
    cout << "\n--- Starting gameplay demo ---" << endl;
    
    // First round - everyone gathers coins
    governor->gather();
    spy->gather();
    baron->gather();
    general->gather();
    judge->gather();
    
    // Try to make an illegal move (not spy's turn)
    cout << "\nTrying to make an illegal move (not spy's turn):" << endl;
    try{
        spy->gather();
    } catch (const std::exception &e){
        std::cerr << "Error: " << e.what() << '\n';
    }
    
    // Second round
    governor->gather(); // Now Moshe has 2 coins
    spy->tax();         // Yossi uses tax to get 2 coins (total 3)
    
    // Try to have Judge undo Governor's action (should fail as Judge can only undo bribe)
    cout << "\nTrying to have Judge undo Governor's gather (should fail):" << endl;
    try {
        judge->undo(*governor);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
    
    cout << governor->coins() << endl; // Expected: 2
    cout << spy->coins() << endl; // Expected: 3
    governor->undo(*spy); // Governor undo tax
    cout << spy->coins() << endl; // Expected: 1
    baron->tax();
    general->gather();
    judge->gather(); 
    governor->tax();
    spy->gather();
    baron->invest(); // Baron traded its 3 coins and got 6 
    general->gather();
    judge->gather();
    
    cout << baron->coins() << endl; // Expected: 6
    governor->tax();
    spy->gather();
    baron->gather();
    general->gather();
    judge->gather();
    governor->tax();
    spy->gather();
    cout << baron->coins() << endl; // Expected: 7
    baron->coup(*governor); // Coup against governor
    general->gather();
    judge->gather();
    
    players = game_1.players();
    // Since no one blocked the Baron, the expected output is:
    // Yossi
    // Meirav
    // Reut
    // Gilad
    cout << "\nActive players after the coup:" << endl;
    for (string name : players) {
        cout << name << endl;
    }
    
    cout << "\n--- End of demo ---" << endl;
    
    return 0;
}