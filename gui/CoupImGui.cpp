// Full GUI version for Coup game with full logic, random role assignment, and in-game actions

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

        if (!gameStarted) {
            ImGui::Text("Add players to begin:");
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
                        } catch (const std::exception& e) {
                            errorMessage = e.what();
                        }
                    }
                }
            }

            if (ImGui::Button("Start Game") && players.size() >= 2) {
                try {
                    game.startGame();
                    gameStarted = true;
                    currentTurn = game.turn();
                    turnCount = 1;
                } catch (const std::exception& e) {
                    errorMessage = e.what();
                }
            }

            ImGui::Separator();
            ImGui::Text("Players:");
            for (const auto& player : players) {
                ImGui::BulletText("%s (%s)", player->getName().c_str(), player->role().c_str());
            }
        } else {
            ImGui::Text("Current Turn: %s", currentTurn.c_str());
            ImGui::Text("Turn Count: %d", turnCount);
        }

        ImGui::Separator();
        if (!errorMessage.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Error: %s", errorMessage.c_str());
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
