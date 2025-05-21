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
MAIN_SRC = test/Main.cpp # Correct path to your Main.cpp
CLASS_SRCS = $(filter-out $(MAIN_SRC), $(wildcard $(SRC_DIR)/*.cpp))
CLASS_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLASS_SRCS))
MAIN_OBJ = $(OBJ_DIR)/Main.o # Note: capitalized 'M' in Main.o

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

all: Main TestExec CoupGUI

# Main executable - explicitly include Main.o
Main: $(MAIN_OBJ) $(CLASS_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	@echo "Main built successfully"

# Test executable
TestExec: $(CLASS_OBJS) $(OBJ_DIR)/Test.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	chmod +x $@

# GUI executable
GUI_SOURCE = $(GUI_DIR)/CoupImGui.cpp
CoupGUI: $(CLASS_OBJS) $(IMGUI_OBJECTS) $(GUI_SOURCE)
	$(CXX) $(CXXFLAGS) $(GUI_SOURCE) $(CLASS_OBJS) $(IMGUI_OBJECTS) -o $@ $(LDLIBS)

# Explicitly compile Main.cpp
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/Test.o: $(TEST_DIR)/Test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile ImGui sources
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run targets
test: TestExec
	./TestExec

# Valgrind for memory check
valgrind: TestExec
	valgrind --leak-check=full --error-exitcode=1 ./TestExec

# Clean
clean:
	rm -f Main TestExec CoupGUI
	rm -f $(OBJ_DIR)/*.o
	rm -f $(IMGUI_OBJECTS)