// Fixed GUI version for Coup game with full logic, random role assignment, and in-game actions

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>
#include <functional>
#include <map>

#include "../include/Game.hpp"
#include "../include/Player.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.20f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.22f, 1.00f);
    style.FrameRounding = 4.0f;
    style.WindowRounding = 6.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowPadding = ImVec2(15, 15);
    style.ItemSpacing = ImVec2(12, 8);
    style.FramePadding = ImVec2(8, 6);
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Coup Game", NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#ifdef ImGuiConfigFlags_DockingEnable
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

    SetupImGuiStyle();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Game state
    coup::Game game;
    std::vector<std::shared_ptr<coup::Player>> players;
    std::string currentTurn;
    std::string errorMessage;
    bool gameStarted = false;
    bool gameOver = false;
    bool showingRoleExplanations = false;
    bool showingGeneralRules = false;
    bool showingTips = false; // Controls whether to show tips during game
    std::string winner;

    static char playerName[64] = "";
    int turnCount = 0;
    std::mt19937 rng(static_cast<unsigned>(time(nullptr)));

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Coup Game");

        if (!gameStarted && !showingGeneralRules) {
            ImGui::Text("COUP GAME");
            ImGui::Separator();
            
            // How to Play button
            if (ImGui::Button("How to Play")) {
                showingGeneralRules = true;
            }
            ImGui::SameLine();
            ImGui::Text(" <- Click if you're new to the game");
            
            ImGui::Separator();
            ImGui::Text("Add players to begin (minimum 2 players):");
            ImGui::InputText("Player Name", playerName, IM_ARRAYSIZE(playerName));

            if (ImGui::Button("Add Player")) {
                if (strlen(playerName) > 0) {
                    int roleIndex = rng() % 6;
                    std::shared_ptr<coup::Player> newPlayer;
                    switch (roleIndex) {
                        case 0: newPlayer = std::make_shared<coup::Governor>(game, playerName); break;
                        case 1: newPlayer = std::make_shared<coup::Spy>(game, playerName); break;
                        case 2: newPlayer = std::make_shared<coup::Baron>(game, playerName); break;
                        case 3: newPlayer = std::make_shared<coup::General>(game, playerName); break;
                        case 4: newPlayer = std::make_shared<coup::Judge>(game, playerName); break;
                        case 5: newPlayer = std::make_shared<coup::Merchant>(game, playerName); break;
                    }
                    if (newPlayer) {
                        try {
                            game.addPlayer(newPlayer);
                            players.push_back(newPlayer);
                            playerName[0] = '\0';
                            errorMessage.clear();
                        } catch (const std::exception& e) {
                            errorMessage = e.what();
                        }
                    }
                }
            }

            if (players.size() >= 2) {
                ImGui::Separator();
                
                // Two clear options
                ImGui::Text("Choose game mode:");
                
                if (ImGui::Button("Beginner Mode (with help & tips)")) {
                    try {
                        game.startGame();
                        showingRoleExplanations = true; // Show role explanations first
                        showingTips = true; // Enable tips during game
                        gameStarted = false; // Keep false until after role explanations
                        currentTurn = game.turn();
                        turnCount = 1;
                        errorMessage.clear();
                    } catch (const std::exception& e) {
                        errorMessage = e.what();
                    }
                }
                
                if (ImGui::Button("Expert Mode (clean interface)")) {
                    try {
                        game.startGame();
                        showingRoleExplanations = false; // Skip explanations
                        showingTips = false; // No tips during game
                        gameStarted = true; // Go directly to game
                        currentTurn = game.turn();
                        turnCount = 1;
                        errorMessage.clear();
                    } catch (const std::exception& e) {
                        errorMessage = e.what();
                    }
                }
                
                ImGui::Separator();
                ImGui::Text("Beginner: Role explanations + in-game help");
                ImGui::Text("Expert: Clean interface, no distractions");
            }

            ImGui::Separator();
            ImGui::Text("Players and their roles:");
            for (const auto& player : players) {
                ImGui::BulletText("%s (%s) - %d coins", 
                                player->getName().c_str(), 
                                player->role().c_str(), 
                                player->coins());
                
                // Add role explanation on hover
                if (ImGui::IsItemHovered()) {
                    std::string tooltip = "Role: " + player->role() + "\n";
                    if (player->role() == "Governor") tooltip += "- Tax: Gets 3 coins\n- Can undo other's tax";
                    else if (player->role() == "Spy") tooltip += "- SpyOn: See coins, block arrest\n- Arrest: Target loses 1 coin";
                    else if (player->role() == "Baron") tooltip += "- Invest: 3 coins -> 6 coins\n- Bribe: Costs 4 coins";
                    else if (player->role() == "General") tooltip += "- Defend: Protect from coups (5 coins)\n- Gets compensation when arrested";
                    else if (player->role() == "Judge") tooltip += "- Sanction: Blocks target's actions\n- Can block bribes";
                    else if (player->role() == "Merchant") tooltip += "- Bonus coin with 3+ coins\n- Pays 2 when arrested";
                    
                    ImGui::SetTooltip("%s", tooltip.c_str());
                }
            }
            
        } else if (showingGeneralRules) {
            // General rules explanation screen
            ImGui::Text("HOW TO PLAY COUP");
            ImGui::Separator();
            
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "GOAL:");
            ImGui::Text("Be the last player standing!");
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "BASIC ACTIONS (Everyone can do):");
            ImGui::BulletText("Gather: Get 1 coin (safe action)");
            ImGui::BulletText("Coup: Pay 7 coins to eliminate a player");
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "SPECIAL ROLES:");
            ImGui::BulletText("Governor: Tax for 3 coins, can undo others' tax");
            ImGui::BulletText("Spy: Spy on others (free), arrest for 1 coin");
            ImGui::BulletText("Baron: Invest 3->6 coins, can bribe");
            ImGui::BulletText("General: Defend from coups (5 coins)");
            ImGui::BulletText("Judge: Sanction players, block bribes");
            ImGui::BulletText("Merchant: Bonus coins, special arrest rules");
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "IMPORTANT RULES:");
            ImGui::BulletText("10+ coins = MUST coup (no choice!)");
            ImGui::BulletText("7+ coins = Can coup");
            ImGui::BulletText("Some actions can be blocked by other players");
            ImGui::BulletText("Each role has unique strengths and weaknesses");
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 1.0f, 1.0f), "BEGINNER TIPS:");
            ImGui::BulletText("Start by gathering coins to learn the game");
            ImGui::BulletText("Watch what other players can do");
            ImGui::BulletText("Don't rush to coup - timing is important");
            ImGui::BulletText("Each role plays differently - experiment!");
            
            ImGui::Separator();
            if (ImGui::Button("Got it! Back to Setup")) {
                showingGeneralRules = false;
            }
            
        } else if (showingRoleExplanations) {
            // Role explanations screen
            ImGui::Text("ROLE EXPLANATIONS");
            ImGui::Text("Learn what each player can do before the game starts!");
            ImGui::Separator();
            
            for (const auto& player : players) {
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), 
                                 "%s - You are a %s", 
                                 player->getName().c_str(), 
                                 player->role().c_str());
                
                if (player->role() == "Governor") {
                    ImGui::Text("  Powers:");
                    ImGui::BulletText("Tax: Get 3 coins instead of 2 (others get 2)");
                    ImGui::BulletText("Undo: Cancel another player's tax action");
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "  Strategy: Use tax for quick coins, undo to disrupt others");
                }
                else if (player->role() == "Spy") {
                    ImGui::Text("  Powers:");
                    ImGui::BulletText("SpyOn: See target's coins + block their next arrest (FREE - doesn't end turn)");
                    ImGui::BulletText("Arrest: Target loses 1 coin to you");
                    ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.8f, 1.0f), "  Strategy: Spy first to gather info and protect yourself");
                }
                else if (player->role() == "Baron") {
                    ImGui::Text("  Powers:");
                    ImGui::BulletText("Invest: If you have EXACTLY 3 coins, get 6 coins total");
                    ImGui::BulletText("Bribe: Pay 4 coins for advantage (Judge can block this)");
                    ImGui::BulletText("Compensation: When sanctioned, lose only 1 coin max");
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "  Strategy: Save up to exactly 3 coins, then invest for big gains");
                }
                else if (player->role() == "General") {
                    ImGui::Text("  Powers:");
                    ImGui::BulletText("Defend: Pay 5 coins to protect anyone from next coup");
                    ImGui::BulletText("Compensation: When arrested, you don't lose the coin");
                    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "  Strategy: Save coins to protect yourself or allies from coups");
                }
                else if (player->role() == "Judge") {
                    ImGui::Text("  Powers:");
                    ImGui::BulletText("Sanction: Prevent target from gathering coins or using tax");
                    ImGui::BulletText("Block Bribe: Cancel Baron's bribe and make them lose 4 coins");
                    ImGui::BulletText("Compensation: Attackers pay extra when sanctioning you");
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "  Strategy: Control the game by blocking key actions");
                }
                else if (player->role() == "Merchant") {
                    ImGui::Text("  Powers (All Automatic):");
                    ImGui::BulletText("Bonus: Get +1 coin when starting turn with 3+ coins");
                    ImGui::BulletText("Penalty: When arrested, pay 2 coins to treasury (not to attacker)");
                    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "  Strategy: Maintain 3+ coins for steady income");
                }
                
                ImGui::Separator();
            }
            
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.2f, 1.0f), "IMPORTANT REMINDERS:");
            ImGui::BulletText("You need 7+ coins to coup someone (eliminates them)");
            ImGui::BulletText("If you get 10+ coins, you MUST coup someone");
            ImGui::BulletText("Last player standing wins!");
            
            ImGui::Separator();
            
            // Optional role help button
            if (ImGui::Button("Learn Your Roles")) {
                // Keep showing role explanations
            }
            ImGui::SameLine();
            if (ImGui::Button("Skip - Let's Play!")) {
                showingRoleExplanations = false;
                gameStarted = true;
            }
            
            ImGui::Text(" <- Click 'Learn Your Roles' if you want detailed explanations");
            
        } else if (!gameOver) {
            if (showingTips) {
                // BEGINNER MODE - Game interface with help section
                ImGui::Columns(2, "GameLayout", true);
                
                // Left column - Game state and actions
                ImGui::Text("TURN %d", turnCount);
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Current: %s", currentTurn.c_str());
                
                // Show next player
                auto nextPlayerIt = std::find_if(players.begin(), players.end(),
                    [&](const std::shared_ptr<coup::Player>& p) { return p->getName() == currentTurn; });
                if (nextPlayerIt != players.end()) {
                    auto nextIt = std::next(nextPlayerIt);
                    if (nextIt == players.end()) nextIt = players.begin();
                    ImGui::Text("Next: %s", (*nextIt)->getName().c_str());
                }
                
                ImGui::Separator();

                // Display all players with better formatting
                ImGui::Text("PLAYERS");
                for (const auto& player : players) {
                    bool isCurrentPlayer = (game.turn() == player->getName());
                    
                    // Show player with status indicators
                    std::string playerText = player->getName() + " (" + player->role() + ") - " + 
                                           std::to_string(player->coins()) + " coins";
                                           
                    if (isCurrentPlayer) {
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), ">>> %s", playerText.c_str());
                    } else {
                        ImGui::Text("    %s", playerText.c_str());
                    }
                    
                    // Add status warnings
                    if (player->coins() >= 10) {
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "[MUST COUP!]");
                    } else if (player->coins() >= 7) {
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[Can Coup]");
                    }
                }
            } else {
                // EXPERT MODE - Clean interface without tips
                ImGui::Text("Turn %d - Current Player: %s", turnCount, currentTurn.c_str());
                ImGui::Separator();

                // Simple player display
                ImGui::Text("Players:");
                for (const auto& player : players) {
                    bool isCurrentPlayer = (game.turn() == player->getName());
                    if (isCurrentPlayer) {
                        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), 
                                         "-> %s (%s) - %d coins", 
                                         player->getName().c_str(), 
                                         player->role().c_str(), 
                                         player->coins());
                    } else {
                        ImGui::Text("   %s (%s) - %d coins", 
                                  player->getName().c_str(), 
                                  player->role().c_str(), 
                                  player->coins());
                    }
                }
            }

            ImGui::Separator();

            // Current player actions
            for (auto& player : players) {
                if (game.turn() == player->getName()) {
                    ImGui::Text("%s's Actions:", player->getName().c_str());

                    // Check if player must coup (10+ coins)
                    if (player->coins() >= 10) {
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                                         "You must perform a coup (10+ coins)!");
                        
                        for (const auto& target : players) {
                            if (target != player) {
                                std::string buttonText = "Coup " + target->getName();
                                if (ImGui::Button(buttonText.c_str())) {
                                    try {
                                        player->coup(*target);
                                        
                                        // Remove couped player
                                        players.erase(std::remove(players.begin(), players.end(), target), 
                                                    players.end());
                                        
                                        currentTurn = game.turn();
                                        turnCount++;
                                        errorMessage.clear();
                                    } catch (const std::exception& e) {
                                        errorMessage = e.what();
                                    }
                                }
                            }
                        }
                    } else {
                        // Basic actions - Only some are available to all players
                        ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Basic Actions:");
                        if (ImGui::Button("Gather (1 coin) - Safe action")) {
                            try {
                                player->gather();
                                errorMessage.clear();
                                currentTurn = game.turn();
                                turnCount++;
                            } catch (const std::exception& e) {
                                errorMessage = e.what();
                            }
                        }

                        // General actions available to all (but not role-specific)
                        if (player->coins() >= 7) {
                            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1.0f), "Coup Actions (Eliminates target!):");
                            for (const auto& target : players) {
                                if (target != player) {
                                    std::string buttonText = "COUP " + target->getName() + " (costs 7 coins)";
                                    if (ImGui::Button(buttonText.c_str())) {
                                        try {
                                            player->coup(*target);
                                            
                                            // Remove couped player
                                            players.erase(std::remove(players.begin(), players.end(), target), 
                                                        players.end());
                                            
                                            currentTurn = game.turn();
                                            turnCount++;
                                            errorMessage.clear();
                                        } catch (const std::exception& e) {
                                            errorMessage = e.what();
                                        }
                                    }
                                }
                            }
                        }

                        // Role-specific actions - Updated to match EXACT game rules
                        ImGui::Separator();
                        ImGui::Text("Special Abilities:");

                        // GOVERNOR ABILITIES - Tax + Undo
                        if (player->role() == "Governor") {
                            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Governor Powers:");
                            
                            // Tax - Governor exclusive, gets 3 coins
                            if (ImGui::Button("Tax (3 coins - Governor only)")) {
                                try {
                                    auto gov = std::dynamic_pointer_cast<coup::Governor>(player);
                                    if (gov) {
                                        gov->tax();
                                        errorMessage.clear();
                                        currentTurn = game.turn();
                                        turnCount++;
                                    }
                                } catch (const std::exception& e) {
                                    errorMessage = e.what();
                                }
                            }
                            
                            // Undo tax actions
                            for (auto& target : players) {
                                if (target != player) {
                                    std::string buttonText = "Undo " + target->getName() + "'s Tax";
                                    if (ImGui::Button(buttonText.c_str())) {
                                        try {
                                            auto gov = std::dynamic_pointer_cast<coup::Governor>(player);
                                            if (gov) {
                                                gov->undo(*target);
                                                errorMessage.clear();
                                            }
                                        } catch (const std::exception& e) {
                                            errorMessage = e.what();
                                        }
                                    }
                                }
                            }
                        }

                        // SPY ABILITIES - Both spyOn and arrest
                        else if (player->role() == "Spy") {
                            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.8f, 1.0f), "Spy Powers:");
                            
                            // SpyOn - doesn't end turn, no cost
                            ImGui::Text("SpyOn (doesn't end turn, free):");
                            for (auto& target : players) {
                                if (target != player) {
                                    std::string spyText = "Spy On " + target->getName() + " (See coins + Block their arrest)";
                                    if (ImGui::Button(spyText.c_str())) {
                                        try {
                                            auto spy = std::dynamic_pointer_cast<coup::Spy>(player);
                                            if (spy) {
                                                spy->spyOn(*target);
                                                errorMessage = "Spied on " + target->getName() + 
                                                             " (has " + std::to_string(target->coins()) + 
                                                             " coins) and blocked their next arrest!";
                                                // Note: Turn doesn't advance for spyOn
                                                // currentTurn = game.turn();
                                            }
                                        } catch (const std::exception& e) {
                                            errorMessage = e.what();
                                        }
                                    }
                                }
                            }
                            
                            ImGui::Separator();
                            
                            // Arrest - normal action, ends turn
                            ImGui::Text("Arrest (ends turn):");
                            for (auto& target : players) {
                                if (target != player) {
                                    std::string arrestText = "Arrest " + target->getName() + " (-1 coin to them)";
                                    if (ImGui::Button(arrestText.c_str())) {
                                        try {
                                            auto spy = std::dynamic_pointer_cast<coup::Spy>(player);
                                            if (spy) {
                                                spy->arrest(*target);
                                                errorMessage.clear();
                                                currentTurn = game.turn();
                                                turnCount++;
                                            }
                                        } catch (const std::exception& e) {
                                            errorMessage = e.what();
                                        }
                                    }
                                }
                            }
                        }

                        // BARON ABILITIES - Invest (exactly 3 coins) + Bribe
                        else if (player->role() == "Baron") {
                            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Baron Powers:");
                            
                            // Invest - requires exactly 3 coins
                            bool canInvest = (player->coins() == 3);
                            if (!canInvest) {
                                ImGui::BeginDisabled();
                            }
                            
                            if (ImGui::Button("Invest (EXACTLY 3 coins -> get 6)")) {
                                try {
                                    auto baron = std::dynamic_pointer_cast<coup::Baron>(player);
                                    if (baron) {
                                        baron->invest();
                                        errorMessage.clear();
                                        currentTurn = game.turn();
                                        turnCount++;
                                    }
                                } catch (const std::exception& e) {
                                    errorMessage = e.what();
                                }
                            }
                            
                            if (!canInvest) {
                                ImGui::EndDisabled();
                                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                                                 "Need EXACTLY 3 coins to invest (you have %d)", 
                                                 player->coins());
                            }

                            // Bribe - costs 4 coins (can be blocked by Judge)
                            bool canBribe = (player->coins() >= 4);
                            if (!canBribe) {
                                ImGui::BeginDisabled();
                            }
                            
                            if (ImGui::Button("Bribe (4 coins - can be blocked by Judge)")) {
                                try {
                                    auto baron = std::dynamic_pointer_cast<coup::Baron>(player);
                                    if (baron) {
                                        baron->bribe();
                                        errorMessage.clear();
                                        currentTurn = game.turn();
                                        turnCount++;
                                    }
                                } catch (const std::exception& e) {
                                    errorMessage = e.what();
                                }
                            }
                            
                            if (!canBribe) {
                                ImGui::EndDisabled();
                                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                                                 "Need 4+ coins to bribe (you have %d)", 
                                                 player->coins());
                            }
                            
                            ImGui::Text("Passive: Gets compensation when sanctioned");
                        }

                        // GENERAL ABILITIES - Coup Defense
                        else if (player->role() == "General") {
                            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "General Powers:");
                            
                            bool canDefend = (player->coins() >= 5);
                            if (!canDefend) {
                                ImGui::BeginDisabled();
                            }
                            
                            for (auto& target : players) {
                                std::string defendText = "Defend " + target->getName() + " from Coup (5 coins)";
                                if (ImGui::Button(defendText.c_str())) {
                                    try {
                                        auto general = std::dynamic_pointer_cast<coup::General>(player);
                                        if (general) {
                                            general->prepareCoupDefense(*target);
                                            errorMessage = "Protected " + target->getName() + " from next coup!";
                                            currentTurn = game.turn();
                                            turnCount++;
                                        }
                                    } catch (const std::exception& e) {
                                        errorMessage = e.what();
                                    }
                                }
                            }
                            
                            if (!canDefend) {
                                ImGui::EndDisabled();
                                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                                                 "Need 5+ coins to defend (you have %d)", 
                                                 player->coins());
                            }
                            
                            ImGui::Text("Passive: Gets compensation when arrested");
                        }

                        // JUDGE ABILITIES - Sanction + Block Bribe
                        else if (player->role() == "Judge") {
                            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Judge Powers:");
                            
                            for (auto& target : players) {
                                if (target != player) {
                                    std::string sanctionText = "Sanction " + target->getName() + " (prevents tax/gather)";
                                    if (ImGui::Button(sanctionText.c_str())) {
                                        try {
                                            auto judge = std::dynamic_pointer_cast<coup::Judge>(player);
                                            if (judge) {
                                                judge->sanction(*target);
                                                errorMessage.clear();
                                                currentTurn = game.turn();
                                                turnCount++;
                                            }
                                        } catch (const std::exception& e) {
                                            errorMessage = e.what();
                                        }
                                    }

                                    std::string undoBribeText = "Block " + target->getName() + "'s Bribe";
                                    if (ImGui::Button(undoBribeText.c_str())) {
                                        try {
                                            auto judge = std::dynamic_pointer_cast<coup::Judge>(player);
                                            if (judge) {
                                                judge->undo(*target);
                                                errorMessage = "Blocked " + target->getName() + "'s bribe - they lose 4 coins!";
                                            }
                                        } catch (const std::exception& e) {
                                            errorMessage = e.what();
                                        }
                                    }
                                }
                            }
                            
                            ImGui::Text("Passive: Attacker pays extra coin when sanctioning Judge");
                        }

                        // MERCHANT ABILITIES - All passive
                        else if (player->role() == "Merchant") {
                            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Merchant Powers:");
                            ImGui::Text("All abilities are passive:");
                            ImGui::Text("- Gets +1 coin when starting turn with 3+ coins");
                            ImGui::Text("- Pays 2 coins to treasury when arrested");
                            if (player->coins() >= 3) {
                                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), 
                                                 "Will get bonus coin next turn!");
                            }
                        }

                        // Unknown role
                        else {
                            ImGui::Text("No special abilities for this role");
                        }
                    }
                    break; // Only show actions for current player
                }
            }

            // Show help panel only in beginner mode
            if (showingTips) {
                ImGui::NextColumn();
                
                // Right column - Help and game info
                ImGui::Text("QUICK HELP");
                
                // Current player help
                auto helpPlayerIt = std::find_if(players.begin(), players.end(),
                    [&](const std::shared_ptr<coup::Player>& p) { return p->getName() == currentTurn; });
                if (helpPlayerIt != players.end()) {
                    auto currentPlayer = *helpPlayerIt;
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "%s's Quick Guide:", currentPlayer->getName().c_str());
                    
                    if (currentPlayer->role() == "Governor") {
                        ImGui::BulletText("Tax gives you 3 coins!");
                        ImGui::BulletText("You can undo others' tax");
                    } else if (currentPlayer->role() == "Spy") {
                        ImGui::BulletText("SpyOn first (free action)");
                        ImGui::BulletText("Then arrest or gather");
                    } else if (currentPlayer->role() == "Baron") {
                        if (currentPlayer->coins() == 3) {
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Perfect! You can invest!");
                        } else {
                            ImGui::BulletText("Need exactly 3 coins to invest");
                        }
                    } else if (currentPlayer->role() == "General") {
                        if (currentPlayer->coins() >= 5) {
                            ImGui::BulletText("You can defend against coups!");
                        } else {
                            ImGui::BulletText("Save up to 5 coins for defense");
                        }
                    } else if (currentPlayer->role() == "Judge") {
                        ImGui::BulletText("Sanction to control others");
                        ImGui::BulletText("Block bribes for free");
                    } else if (currentPlayer->role() == "Merchant") {
                        if (currentPlayer->coins() >= 3) {
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "You got bonus coin!");
                        }
                        ImGui::BulletText("Keep 3+ coins for bonuses");
                    }
                }
                
                ImGui::Separator();
                ImGui::Text("GAME STATUS");
                ImGui::Text("Players left: %d", (int)players.size());
                ImGui::Text("Turns played: %d", turnCount - 1);
                
                // Threat analysis
                ImGui::Separator();
                ImGui::Text("THREATS");
                bool anyThreats = false;
                for (const auto& player : players) {
                    if (player->coins() >= 7) {
                        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), 
                                         "%s can coup! (%d coins)", 
                                         player->getName().c_str(), 
                                         player->coins());
                        anyThreats = true;
                    }
                }
                if (!anyThreats) {
                    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "No immediate threats");
                }
                
                ImGui::Columns(1);
            }

            // Check for winner
            try {
                winner = game.winner();
                gameOver = true;
            } catch (...) {
                // Game still ongoing
            }

            // Out-of-turn actions (blocking abilities)
            ImGui::Separator();
            ImGui::Text("Blocking Actions (Available anytime):");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                             "These don't require your turn and can block other actions:");

            for (const auto& blockingPlayer : players) {
                // Governor can undo tax actions
                if (blockingPlayer->role() == "Governor" && blockingPlayer->getName() != currentTurn) {
                    for (const auto& target : players) {
                        if (target != blockingPlayer) {
                            std::string blockText = blockingPlayer->getName() + ": Block " + target->getName() + "'s Tax";
                            if (ImGui::Button(blockText.c_str())) {
                                try {
                                    auto gov = std::dynamic_pointer_cast<coup::Governor>(blockingPlayer);
                                    if (gov) {
                                        gov->undo(*target);
                                        errorMessage = blockingPlayer->getName() + " blocked " + target->getName() + "'s tax!";
                                    }
                                } catch (const std::exception& e) {
                                    errorMessage = e.what();
                                }
                            }
                        }
                    }
                }

                // Judge can block bribes
                if (blockingPlayer->role() == "Judge" && blockingPlayer->getName() != currentTurn) {
                    for (const auto& target : players) {
                        if (target != blockingPlayer) {
                            std::string blockText = blockingPlayer->getName() + ": Block " + target->getName() + "'s Bribe";
                            if (ImGui::Button(blockText.c_str())) {
                                try {
                                    auto judge = std::dynamic_pointer_cast<coup::Judge>(blockingPlayer);
                                    if (judge) {
                                        judge->undo(*target);
                                        errorMessage = blockingPlayer->getName() + " blocked " + target->getName() + "'s bribe!";
                                    }
                                } catch (const std::exception& e) {
                                    errorMessage = e.what();
                                }
                            }
                        }
                    }
                }
            }
            
        } else {
            // Game over
            ImGui::Text("Game Over!");
            ImGui::Text("Winner: %s", winner.c_str());
            
            if (ImGui::Button("New Game")) {
                // Reset game state
                game = coup::Game();
                players.clear();
                gameStarted = false;
                gameOver = false;
                showingRoleExplanations = false;
                showingGeneralRules = false;
                showingTips = false;
                currentTurn.clear();
                winner.clear();
                turnCount = 0;
                errorMessage.clear();
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Show Final Stats")) {
                // Could add game statistics here
                errorMessage = "Game lasted " + std::to_string(turnCount - 1) + " turns. " + 
                              winner + " played excellently!";
            }
        }

        // Error messages
        if (!errorMessage.empty()) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error: %s", errorMessage.c_str());
            
            if (ImGui::Button("Clear Error")) {
                errorMessage.clear();
            }
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}