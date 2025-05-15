// Your email address here

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/decostet.h"
#include "../include/Player.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"
#include "../include/Game.hpp"
#include "../include/Exceptions.hpp"

using namespace coup;

TEST_CASE("Game basics") {
    Game game;
    
    // Create players
    Governor governor(game, "Alice");
    Spy spy(game, "Bob");
    
    // Test player list
    CHECK(game.players().size() == 2);
    CHECK(game.players()[0] == "Alice");
    CHECK(game.players()[1] == "Bob");
    
    // Test turn management
    CHECK(game.turn() == "Alice");
    governor.gather();
    CHECK(game.turn() == "Bob");
    spy.gather();
    CHECK(game.turn() == "Alice");
    
    // Test coin management
    CHECK(governor.coins() == 1);
    CHECK(spy.coins() == 1);
}

TEST_CASE("Governor abilities") {
    Game game;
    
    Governor governor(game, "Alice");
    Spy spy(game, "Bob");
    
    // Test that Governor gets 3 coins from tax instead of 2
    governor.tax();
    CHECK(governor.coins() == 3);
    
    // Test that Governor can undo tax
    spy.tax();
    CHECK(spy.coins() == 2);
    governor.undo(spy);
    CHECK(spy.coins() == 0);
}

TEST_CASE("Baron abilities") {
    Game game;
    
    Baron baron(game, "Alice");
    Judge judge(game, "Bob");
    
    // Give Baron some initial coins
    baron.gather();
    judge.gather();
    baron.gather();
    judge.gather();
    baron.gather();
    
    // Test Baron's invest ability
    CHECK(baron.coins() == 3);
    baron.invest();
    CHECK(baron.coins() == 6);
    
    // Test Baron's sanction compensation
    judge.gather();
    baron.gather();
    judge.gather();
    baron.gather();
    
    // Judge gets enough coins for sanction
    judge.gather();
    baron.gather();
    
    int baronCoins = baron.coins();
    judge.sanction(baron);
    
    // Baron should lose at most 1 coin (not 3) due to compensation
    CHECK(baron.coins() == baronCoins - 1);
}

TEST_CASE("General abilities") {
    Game game;
    
    General general(game, "Alice");
    Spy spy(game, "Bob");
    Baron baron(game, "Charlie");
    
    // Give everyone coins
    general.gather();
    spy.gather();
    baron.gather();
    
    general.gather();
    spy.gather();
    baron.gather();
    
    general.gather();
    spy.gather();
    baron.gather();
    
    // Test General getting arrested
    general.gather();
    spy.gather();
    baron.gather();
    
    general.gather();
    spy.gather();
    baron.gather();
    
    general.gather();
    spy.gather();
    baron.gather();
    
    general.gather();
    
    int generalCoins = general.coins();
    spy.arrest(general);
    
    // General should get the coin back after being arrested
    CHECK(general.coins() == generalCoins);
}

TEST_CASE("Judge abilities") {
    Game game;
    
    Judge judge(game, "Alice");
    Baron baron(game, "Bob");
    
    // Give everyone coins
    judge.gather();
    baron.gather();
    
    judge.gather();
    baron.gather();
    
    judge.gather();
    baron.gather();
    
    judge.gather();
    
    // Test Judge blocking bribe
    baron.bribe();
    judge.undo(baron);
    
    // Baron should have lost the 4 coins from bribe but gained nothing
    CHECK(baron.coins() == 0);
}

TEST_CASE("Merchant abilities") {
    Game game;
    
    Merchant merchant(game, "Alice");
    Spy spy(game, "Bob");
    
    // Give merchant coins
    merchant.gather();
    spy.gather();
    merchant.gather();
    spy.gather();
    merchant.gather();
    
    // Test merchant getting extra coin at start of turn
    CHECK(merchant.coins() == 3);
    spy.gather();
    
    // Merchant should get +1 coin at start of turn (since has 3+ coins)
    CHECK(merchant.coins() == 4);
    
    // Test merchant paying 2 coins to treasury when arrested
    int merchantCoins = merchant.coins();
    spy.arrest(merchant);
    
    // Merchant should lose 2 coins instead of 1
    CHECK(merchant.coins() == merchantCoins - 2);
}

TEST_CASE("Coup action") {
    Game game;
    
    Governor governor(game, "Alice");
    Spy spy(game, "Bob");
    Baron baron(game, "Charlie");
    
    // Give everyone enough coins for coup
    for (int i = 0; i < 7; i++) {
        governor.gather();
        spy.gather();
        baron.gather();
    }
    
    // Governor performs coup on Spy
    governor.coup(spy);
    
    // Check that Spy is no longer in the game
    std::vector<std::string> players = game.players();
    CHECK(players.size() == 2);
    CHECK(players[0] == "Alice");
    CHECK(players[1] == "Charlie");
    
    // Baron performs coup on Governor
    baron.coup(governor);
    
    // Check that Governor is no longer in the game
    players = game.players();
    CHECK(players.size() == 1);
    CHECK(players[0] == "Charlie");
    
    // Baron should be the winner
    CHECK(game.winner() == "Charlie");
}

TEST_CASE("Exception handling") {
    Game game;
    
    Governor governor(game, "Alice");
    Spy spy(game, "Bob");
    
    // Test not enough coins exception
    CHECK_THROWS_AS(governor.coup(spy), NotEnoughCoinsException);
    
    // Test not your turn exception
    CHECK_THROWS_AS(spy.gather(), NotYourTurnException);
    
    // Give governor enough coins and test 10+ coins rule
    for (int i = 0; i < 10; i++) {
        governor.gather();
        spy.gather();
    }
    
    // Governor must coup with 10+ coins
    CHECK_THROWS_AS(governor.gather(), TooManyCoinsException);
    CHECK_THROWS_AS(governor.tax(), TooManyCoinsException);
    
    // No exception should be thrown for coup
    CHECK_NOTHROW(governor.coup(spy));
    
    // Game winner exception when game is still running
    Game game2;
    Governor gov2(game2, "Alice");
    Spy spy2(game2, "Bob");
    
    CHECK_THROWS_AS(game2.winner(), GameStillRunningException);
}