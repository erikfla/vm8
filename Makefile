CXX := g++
CXXFLAGS := -Wall -std=c++20 -Iinclude

SRC := src
OBJ := build
BIN := bin

.PHONY: all clean run-%

all: $(BIN)/tick_watcher

# Sørg for at bin/ og build/ finnes
$(BIN):
	mkdir -p $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

# Bygg program
$(BIN)/tick_watcher: $(OBJ)/tick_watcher.o $(OBJ)/clock.o | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Objektfiler (avhenger også av mappene)
$(OBJ)/tick_watcher.o: $(SRC)/tick_watcher.cpp $(SRC)/clock.hpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ)/clock.o: $(SRC)/clock.cpp $(SRC)/clock.hpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Kjør
run-%: $(BIN)/%
	@echo "▶️ Kjører $*"
	@./$(BIN)/$*

# Rydd opp
clean:
	rm -rf $(OBJ) $(BIN)
