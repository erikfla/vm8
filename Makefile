CXX := g++
CXXFLAGS := -Wall -std=c++20

BIN := bin
OBJ := build
SRC := src

.PHONY: all clean run

all: $(BIN)/power_switch_demo

$(BIN)/power_switch_demo: $(OBJ)/power_switch_demo.o | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ)/power_switch_demo.o: $(SRC)/power_switch_demo.cpp $(SRC)/power_switch.hpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN):
	mkdir -p $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

run: $(BIN)/power_switch_demo
	@echo "▶️ Kjører power_switch_demo"
	@./$(BIN)/power_switch_demo

clean:
	rm -rf $(BIN) $(OBJ)
