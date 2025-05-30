#tomergal40@gmail.com
# Compiler
CXX = g++
CXXFLAGS = -std=c++2a -Wall -Wextra -Werror -g -Iinclude -Iimgui -Iimgui/backends

# Directories
SRC_DIR = src
OBJ_DIR = obj
GUI_DIR = gui
TEST_DIR = test
IMGUI_DIR = imgui

# Make sure directories exist
$(shell mkdir -p $(OBJ_DIR))

# Source files
MAIN_SRC = test/Main.cpp # Full demo Main.cpp
CLASS_SRCS = $(filter-out $(MAIN_SRC), $(wildcard $(SRC_DIR)/*.cpp))
CLASS_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLASS_SRCS))
MAIN_OBJ = $(OBJ_DIR)/Main.o

# ImGui sources
IMGUI_SOURCES = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

IMGUI_OBJECTS = $(patsubst %.cpp, %.o, $(IMGUI_SOURCES))

# Libraries
LDLIBS = -lglfw -lGL -ldl -lpthread

# Main targets
.PHONY: all clean test valgrind Main

all: MainExec TestExec CoupGUI

# Build Main executable
MainExec: $(MAIN_OBJ) $(CLASS_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	@echo "Main executable built successfully"

# Run Main demo 
Main: MainExec
	@echo "Running Coup Game Demonstration..."
	./MainExec

# Build Test executable
TestExec: $(CLASS_OBJS) $(OBJ_DIR)/Test.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	chmod +x $@
	@echo "Test executable built successfully"

# Run unit tests 
test: TestExec
	@echo "Running unit tests..."
	./TestExec

# Valgrind memory check 
valgrind: TestExec
	@echo "Running valgrind memory check..."
	valgrind --leak-check=full --error-exitcode=1 ./TestExec

# GUI executable
GUI_SOURCE = $(GUI_DIR)/CoupImGui.cpp
CoupGUI: $(CLASS_OBJS) $(IMGUI_OBJECTS) $(GUI_SOURCE)
	$(CXX) $(CXXFLAGS) $(GUI_SOURCE) $(CLASS_OBJS) $(IMGUI_OBJECTS) -o $@ $(LDLIBS)
	@echo "GUI executable built successfully"

# Compilation rules
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/Test.o: $(TEST_DIR)/Test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean 
clean:
	@echo "Cleaning build files..."
	rm -f MainExec TestExec CoupGUI
	rm -f $(OBJ_DIR)/*.o
	rm -f $(IMGUI_OBJECTS)
	@echo "Clean completed"