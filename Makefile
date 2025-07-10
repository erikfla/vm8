BIN := bin
OBJ := build
SRC := src

all: $(BIN)/power_switch_demo

$(BIN)/power_switch_demo: $(OBJ)/power_switch_demo.o | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ)/power_switch_demo.o: $(SRC)/power_switch_demo.cpp $(SRC)/power_switch.hpp | $(OBJ)
	$(CXX) $(CXXFLAGS) -c $< -o $@
