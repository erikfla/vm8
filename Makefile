# 🔧 Kompilator og flagg
CXX := g++
CXXFLAGS := -Wall -std=c++20 -Iinclude

# 📁 Mapper
SRC := src
OBJ := build
BIN := bin

# 📄 Filer
DEMOS := tick_watcher latch_demo latch_demo2 clock_latch_demo latch_clock_demo vm8

# 📦 Sørg for mapper
$(shell mkdir -p $(OBJ) $(BIN))

# 🔄 Objektfiler
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 🎯 Spesifikke linker
$(BIN)/tick_watcher: $(OBJ)/tick_watcher.o $(OBJ)/clock.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/latch_demo: $(OBJ)/latch_demo.o $(OBJ)/latch.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/latch_demo2: $(OBJ)/latch_demo2.o $(OBJ)/latch.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/clock_latch_demo: $(OBJ)/clock_latch_demo.o $(OBJ)/clock.o $(OBJ)/latch.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/latch_clock_demo: $(OBJ)/latch_clock_demo.o $(OBJ)/clock.o $(OBJ)/latch.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/vm8: $(OBJ)/cpu.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# ▶️ Kjør
run-%: $(BIN)/%
	$<

# 🧼 Rydd
clean:
	rm -rf $(OBJ) $(BIN)

.PHONY: all clean $(addprefix run-,$(DEMOS))

# Bygg alle
all: $(addprefix $(BIN)/, $(DEMOS))
