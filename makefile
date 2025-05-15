# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++2a -Wall -Wextra -Werror -g
INCLUDE = -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
TEST_DIR = test

# Source files
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
MAIN_SRC = $(TEST_DIR)/Main.cpp
TEST_SRC = $(TEST_DIR)/Test.cpp

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_FILES))
MAIN_OBJ = $(OBJ_DIR)/Main_test.o  # Renamed to avoid conflict
TEST_OBJ = $(OBJ_DIR)/Test.o

# Executables
MAIN_EXEC = Main
TEST_EXEC = test

# Default target
all: $(MAIN_EXEC) $(TEST_EXEC)

# Main executable
$(MAIN_EXEC): $(OBJ_FILES) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Test executable
$(TEST_EXEC): $(OBJ_FILES) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule for Main.cpp in test directory
$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Rule for normal object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Special rule for Test.cpp
$(TEST_OBJ): $(TEST_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Run main program
.PHONY: run_main
run_main: $(MAIN_EXEC)
	./$(MAIN_EXEC)

# Alias for the assignment's requirement
.PHONY: Main
Main: run_main

# Run tests
.PHONY: run_test
run_test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Alias for the assignment's requirement
.PHONY: test
test: run_test

# Run valgrind
.PHONY: valgrind
valgrind: $(TEST_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_EXEC)

# Clean
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(MAIN_EXEC) $(TEST_EXEC)