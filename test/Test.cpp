// Your email address here

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"
#include "../include/Player.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"
#include <iostream>

using namespace coup;

TEST_CASE("Game basics") {
    Game game;
    
    // Create players as shared_ptr and add them to the game
    auto governor = std::make_shared<Governor>(game, "Alice");
    auto spy = std::make_shared<Spy>(game, "Bob");
    
    // Add players to the game
    game.addPlayer(governor);
    game.addPlayer(spy);
    
    // Start the game - this is crucial!
    game.startGame();
    
    // Test player list
    CHECK(game.players().size() == 2);
    CHECK(game.players()[0] == "Alice");
    CHECK(game.players()[1] == "Bob");
    
    // Test turn management
    CHECK(game.turn() == "Alice");
    governor->gather(); // Note the -> instead of . since we're using shared_ptr
    CHECK(game.turn() == "Bob");
    spy->gather();
    CHECK(game.turn() == "Alice");
    
    // Test coin management
    CHECK(governor->coins() == 1);
    CHECK(spy->coins() == 1);
}

TEST_CASE("Governor abilities") {
    Game game;
    
    // Create and add players
    auto governor = std::make_shared<Governor>(game, "Alice");
    auto spy = std::make_shared<Spy>(game, "Bob");
    
    game.addPlayer(governor);
    game.addPlayer(spy);
    
    // Start the game - this is crucial!
    game.startGame();
    
    std::cout << "Initial turn: " << game.turn() << std::endl;
    
    // Test that Governor gets 3 coins from tax instead of 2
    std::cout << "Governor about to tax" << std::endl;
    governor->tax();
    std::cout << "After governor tax, Governor has " << governor->coins() << " coins" << std::endl;
    CHECK(governor->coins() == 3);
    
    // Test that Governor can undo tax
    std::cout << "Spy about to tax, turn: " << game.turn() << std::endl;
    spy->tax();
    std::cout << "After spy tax, Spy has " << spy->coins() << " coins" << std::endl;
    CHECK(spy->coins() == 2);
    
    std::cout << "Governor about to undo Spy's tax, turn: " << game.turn() << std::endl;
    governor->undo(*spy);
    std::cout << "After undo, Spy has " << spy->coins() << " coins" << std::endl;
    CHECK(spy->coins() == 0);
}

TEST_CASE("Baron abilities") {
    Game game;
    
    // Create and add players
    auto baron = std::make_shared<Baron>(game, "Alice");
    auto judge = std::make_shared<Judge>(game, "Bob");
    
    game.addPlayer(baron);
    game.addPlayer(judge);
    
    // Start the game - this is crucial!
    game.startGame();
    
    // Debug output
    std::cout << "Initial turn: " << game.turn() << std::endl;
    
    // Give Baron some initial coins
    std::cout << "Baron (Alice) about to gather (1)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Judge (Bob) about to gather (1)" << std::endl;
    judge->gather();
    std::cout << "After judge gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron (Alice) about to gather (2)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Judge (Bob) about to gather (2)" << std::endl;
    judge->gather();
    std::cout << "After judge gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron (Alice) about to gather (3)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (3), turn: " << game.turn() << std::endl;
    
    // Test Baron's invest ability
    CHECK(baron->coins() == 3);
    std::cout << "Baron has " << baron->coins() << " coins" << std::endl;
    std::cout << "Current turn before invest: " << game.turn() << std::endl;
    
    // This is where the error occurs
    std::cout << "Baron (Alice) about to invest" << std::endl;
    baron->invest();
    std::cout << "After baron invest, turn: " << game.turn() << std::endl;
    
    CHECK(baron->coins() == 6);
    std::cout << "Baron has " << baron->coins() << " coins after invest" << std::endl;
    
    // Test Baron's sanction compensation
    judge->gather();
    std::cout << "After judge gather (post-invest), turn: " << game.turn() << std::endl;
    
    baron->gather();
    std::cout << "After baron gather (post-invest), turn: " << game.turn() << std::endl;
    
    judge->gather();
    std::cout << "After judge gather (again), turn: " << game.turn() << std::endl;
    
    baron->gather();
    std::cout << "After baron gather (again), turn: " << game.turn() << std::endl;
    
    // Judge gets enough coins for sanction
    judge->gather();
    std::cout << "After judge gather (for sanction), turn: " << game.turn() << std::endl;
    
    baron->gather();
    std::cout << "After baron gather (before sanction), turn: " << game.turn() << std::endl;
    
    int baronCoins = baron->coins();
    std::cout << "Baron has " << baronCoins << " coins before sanction" << std::endl;
    
    std::cout << "Judge (Bob) about to sanction Baron (Alice)" << std::endl;
    judge->sanction(*baron); // Note: sanction takes a reference
    std::cout << "After sanction, turn: " << game.turn() << std::endl;
    
    // Baron should lose at most 1 coin (not 3) due to compensation
    std::cout << "Baron has " << baron->coins() << " coins after sanction" << std::endl;
    CHECK(baron->coins() == baronCoins - 1);
}

TEST_CASE("General abilities") {
    Game game;
    
    // Create and add players
    auto general = std::make_shared<General>(game, "Alice");
    auto spy = std::make_shared<Spy>(game, "Bob");
    auto baron = std::make_shared<Baron>(game, "Charlie");
    
    game.addPlayer(general);
    game.addPlayer(spy);
    game.addPlayer(baron);
    
    // Start the game - this is crucial!
    game.startGame();
    
    std::cout << "Initial turn: " << game.turn() << std::endl;
    
    // Give everyone coins
    std::cout << "General about to gather (1)" << std::endl;
    general->gather();
    std::cout << "After general gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Spy about to gather (1)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (1)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "General about to gather (2)" << std::endl;
    general->gather();
    std::cout << "After general gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Spy about to gather (2)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (2)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "General about to gather (3)" << std::endl;
    general->gather();
    std::cout << "After general gather (3), turn: " << game.turn() << std::endl;
    
    // Test General getting arrested
    std::cout << "Spy about to gather (3)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (3), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (3)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (3), turn: " << game.turn() << std::endl;
    
    std::cout << "General about to gather (4)" << std::endl;
    general->gather();
    std::cout << "After general gather (4), turn: " << game.turn() << std::endl;
    
    std::cout << "Spy about to gather (4)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (4), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (4)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (4), turn: " << game.turn() << std::endl;
    
    std::cout << "General about to gather (5)" << std::endl;
    general->gather();
    std::cout << "After general gather (5), turn: " << game.turn() << std::endl;
    
    std::cout << "Spy about to gather (5)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (5), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (5)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (5), turn: " << game.turn() << std::endl;
    
    std::cout << "General about to gather (6)" << std::endl;
    general->gather();
    std::cout << "After general gather (6), turn: " << game.turn() << std::endl;
    
    std::cout << "General has " << general->coins() << " coins before arrest" << std::endl;
    int generalCoins = general->coins();
    
    std::cout << "Spy about to arrest General, turn: " << game.turn() << std::endl;
    spy->arrest(*general);
    std::cout << "After spy arrests general, turn: " << game.turn() << std::endl;
    
    // General should get the coin back after being arrested
    std::cout << "General has " << general->coins() << " coins after arrest" << std::endl;
    CHECK(general->coins() == generalCoins);
}

TEST_CASE("Judge abilities") {
    Game game;
    
    // Create and add players
    auto judge = std::make_shared<Judge>(game, "Alice");
    auto baron = std::make_shared<Baron>(game, "Bob");
    
    game.addPlayer(judge);
    game.addPlayer(baron);
    
    // Start the game - this is crucial!
    game.startGame();
    
    std::cout << "Initial turn: " << game.turn() << std::endl;
    
    // Give everyone coins
    std::cout << "Judge about to gather (1)" << std::endl;
    judge->gather();
    std::cout << "After judge gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (1)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Judge about to gather (2)" << std::endl;
    judge->gather();
    std::cout << "After judge gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (2)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Judge about to gather (3)" << std::endl;
    judge->gather();
    std::cout << "After judge gather (3), turn: " << game.turn() << std::endl;
    
    std::cout << "Baron about to gather (3)" << std::endl;
    baron->gather();
    std::cout << "After baron gather (3), turn: " << game.turn() << std::endl;
    
    std::cout << "Judge about to gather (4)" << std::endl;
    judge->gather();
    std::cout << "After judge gather (4), turn: " << game.turn() << std::endl;
    
    // Test Judge blocking bribe
    std::cout << "Baron has " << baron->coins() << " coins before bribe" << std::endl;
    std::cout << "Baron about to bribe, turn: " << game.turn() << std::endl;
    baron->bribe();
    std::cout << "After baron bribe, turn: " << game.turn() << std::endl;
    
    std::cout << "Judge about to undo Baron's bribe, turn: " << game.turn() << std::endl;
    judge->undo(*baron);
    std::cout << "After judge undo, turn: " << game.turn() << std::endl;
    
    // Baron should have lost the 4 coins from bribe but gained nothing
    std::cout << "Baron has " << baron->coins() << " coins after undo" << std::endl;
    CHECK(baron->coins() == 0);
}

TEST_CASE("Merchant abilities") {
    Game game;
    
    // Create and add players
    auto merchant = std::make_shared<Merchant>(game, "Alice");
    auto spy = std::make_shared<Spy>(game, "Bob");
    
    game.addPlayer(merchant);
    game.addPlayer(spy);
    
    // Start the game - this is crucial!
    game.startGame();
    
    std::cout << "Initial turn: " << game.turn() << std::endl;
    
    // Give merchant coins
    std::cout << "Merchant about to gather (1)" << std::endl;
    merchant->gather();
    std::cout << "After merchant gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Spy about to gather (1)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (1), turn: " << game.turn() << std::endl;
    
    std::cout << "Merchant about to gather (2)" << std::endl;
    merchant->gather();
    std::cout << "After merchant gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Spy about to gather (2)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (2), turn: " << game.turn() << std::endl;
    
    std::cout << "Merchant about to gather (3)" << std::endl;
    merchant->gather();
    std::cout << "After merchant gather (3), turn: " << game.turn() << std::endl;
    
    // Test merchant getting extra coin at start of turn
    std::cout << "Merchant has " << merchant->coins() << " coins before Spy's turn" << std::endl;
    CHECK(merchant->coins() == 3);
    
    std::cout << "Spy about to gather (3)" << std::endl;
    spy->gather();
    std::cout << "After spy gather (3), turn: " << game.turn() << std::endl;
    
    // Merchant should get +1 coin at start of turn (since has 3+ coins)
    std::cout << "Merchant has " << merchant->coins() << " coins after Spy's turn" << std::endl;
    CHECK(merchant->coins() == 4);
    
    // Test merchant paying 2 coins to treasury when arrested
    int merchantCoins = merchant->coins();
    std::cout << "Merchant has " << merchantCoins << " coins before arrest" << std::endl;
    
    std::cout << "Spy about to arrest Merchant, turn: " << game.turn() << std::endl;
    spy->arrest(*merchant);
    std::cout << "After spy arrests merchant, turn: " << game.turn() << std::endl;
    
    // Merchant should lose 2 coins instead of 1
    std::cout << "Merchant has " << merchant->coins() << " coins after arrest" << std::endl;
    CHECK(merchant->coins() == merchantCoins - 2);
}

TEST_CASE("Coup action") {
    Game game;
    
    // Create and add players
    auto governor = std::make_shared<Governor>(game, "Alice");
    auto spy = std::make_shared<Spy>(game, "Bob");
    auto baron = std::make_shared<Baron>(game, "Charlie");
    
    game.addPlayer(governor);
    game.addPlayer(spy);
    game.addPlayer(baron);
    
    // Start the game - this is crucial!
    game.startGame();
    
    std::cout << "Initial turn: " << game.turn() << std::endl;
    std::cout << "Initial players: ";
    for (const auto& name : game.players()) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    // Give everyone enough coins for coup
    for (int i = 0; i < 7; i++) {
        std::cout << "Governor about to gather (" << (i+1) << ")" << std::endl;
        governor->gather();
        std::cout << "After governor gather, turn: " << game.turn() << std::endl;
        
        std::cout << "Spy about to gather (" << (i+1) << ")" << std::endl;
        spy->gather();
        std::cout << "After spy gather, turn: " << game.turn() << std::endl;
        
        std::cout << "Baron about to gather (" << (i+1) << ")" << std::endl;
        baron->gather();
        std::cout << "After baron gather, turn: " << game.turn() << std::endl;
    }
    
    // Governor performs coup on Spy
    std::cout << "Governor about to coup Spy" << std::endl;
    governor->coup(*spy);
    std::cout << "After governor coups spy, turn: " << game.turn() << std::endl;
    
    // Check that Spy is no longer in the game
    std::cout << "Players after spy is couped: ";
    std::vector<std::string> players = game.players();
    for (const auto& name : players) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    CHECK(players.size() == 2);
    CHECK(players[0] == "Alice");
    CHECK(players[1] == "Charlie");
    
    // Baron performs coup on Governor
    std::cout << "Baron about to coup Governor" << std::endl;
    baron->coup(*governor);
    std::cout << "After baron coups governor, turn: " << game.turn() << std::endl;
    
    // Check that Governor is no longer in the game
    std::cout << "Players after governor is couped: ";
    players = game.players();
    for (const auto& name : players) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
    
    CHECK(players.size() == 1);
    CHECK(players[0] == "Charlie");
    
    // Baron should be the winner
    std::cout << "Winner: " << game.winner() << std::endl;
    CHECK(game.winner() == "Charlie");
}

TEST_CASE("Exception handling") {
    Game game;
    
    // Create and add players
    auto governor = std::make_shared<Governor>(game, "Alice");
    auto spy = std::make_shared<Spy>(game, "Bob");
    
    game.addPlayer(governor);
    game.addPlayer(spy);
    
    // Start the game - this is crucial!
    game.startGame();
    
    std::cout << "Initial turn: " << game.turn() << std::endl;
    
    // Test not enough coins exception
    std::cout << "Testing not enough coins exception:" << std::endl;
    CHECK_THROWS_AS(governor->coup(*spy), NotEnoughCoinsException);
    
    // Test not your turn exception
    std::cout << "Testing not your turn exception:" << std::endl;
    CHECK_THROWS_AS(spy->gather(), NotYourTurnException);
    
    // Give governor enough coins and test 10+ coins rule
    std::cout << "Giving players coins:" << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << "Round " << (i+1) << ": ";
        governor->gather();
        std::cout << "Governor has " << governor->coins() << " coins. ";
        spy->gather();
        std::cout << "Spy has " << spy->coins() << " coins." << std::endl;
    }
    
    // Governor must coup with 10+ coins
    std::cout << "Testing too many coins exception:" << std::endl;
    CHECK_THROWS_AS(governor->gather(), TooManyCoinsException);
    CHECK_THROWS_AS(governor->tax(), TooManyCoinsException);
    
    // No exception should be thrown for coup
    std::cout << "Testing coup with 10+ coins:" << std::endl;
    CHECK_NOTHROW(governor->coup(*spy));
    
    // Game winner exception when game is still running
    Game game2;
    auto gov2 = std::make_shared<Governor>(game2, "Alice");
    auto spy2 = std::make_shared<Spy>(game2, "Bob");
    
    game2.addPlayer(gov2);
    game2.addPlayer(spy2);
    
    // Don't start game2 yet to test winner exception
    
    std::cout << "Testing game still running exception:" << std::endl;
    CHECK_THROWS_AS(game2.winner(), GameStillRunningException);
}