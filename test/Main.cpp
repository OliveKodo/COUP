// Your email address here

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

using namespace std;
using namespace coup;

int main() {
    // Create a game
    Game game;
    
    cout << "Game created" << endl;
    
    // Create players with different roles
    auto governor = make_shared<Governor>(game, "Moshe");
    auto spy = make_shared<Spy>(game, "Yossi");
    auto baron = make_shared<Baron>(game, "Meirav");
    auto general = make_shared<General>(game, "Reut");
    auto judge = make_shared<Judge>(game, "Gilad");
    
    // Add players to the game
    game.addPlayer(governor);
    game.addPlayer(spy);
    game.addPlayer(baron);
    game.addPlayer(general);
    game.addPlayer(judge);
    
    // Start the game
    game.startGame();
    
    cout << "Game started with players: ";
    for (const auto& name : game.players()) {
        cout << name << " ";
    }
    cout << endl;
    
    cout << "Current turn: " << game.turn() << endl;
    
    // Example gameplay
    try {
        // First round: everyone gathers
        governor->gather();
        cout << "Moshe gathered, now has " << governor->coins() << " coins" << endl;
        
        spy->gather();
        cout << "Yossi gathered, now has " << spy->coins() << " coins" << endl;
        
        baron->gather();
        cout << "Meirav gathered, now has " << baron->coins() << " coins" << endl;
        
        general->gather();
        cout << "Reut gathered, now has " << general->coins() << " coins" << endl;
        
        judge->gather();
        cout << "Gilad gathered, now has " << judge->coins() << " coins" << endl;
        
        // Test illegal move
        cout << "\nTrying illegal move (not Yossi's turn):" << endl;
        try {
            spy->gather();
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    
    return 0;
}