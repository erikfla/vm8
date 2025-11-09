# =========================
#  VM8 MAKEFILE
# =========================

CXX := g++
CXXFLAGS := -Wall -std=c++20 -Isrc/core

SRC := src
OBJ := build
BIN := bin

# Sørg for at mappene finnes
$(shell mkdir -p $(OBJ) $(BIN))

# =========================
#  DEMOER / PROGRAMMER
# =========================
DEMOS := vm8 register_demo tick_watcher clock_and_latch_demo power_switch_demo

# =========================
#  STANDARDMÅL
# =========================

.PHONY: all clean run-%

all: $(addprefix $(BIN)/,$(DEMOS))

# =========================
#  HOVEDMASKIN (VM8)
# =========================

$(BIN)/vm8: \
    $(SRC)/demos/vm8_main.cpp \
    $(SRC)/core/machine.cpp \
    $(SRC)/core/clock.cpp \
    $(SRC)/core/latch.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# =========================
#  REGISTER DEMO
# =========================

$(BIN)/register_demo: \
    $(SRC)/demos/register_demo.cpp \
    $(SRC)/core/latch.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# =========================
#  TICK WATCHER
# =========================

$(BIN)/tick_watcher: \
    $(SRC)/demos/tick_watcher.cpp \
    $(SRC)/core/clock.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# =========================
#  CLOCK & LATCH DEMO
# =========================

$(BIN)/clock_and_latch_demo: \
    $(SRC)/demos/clock_and_latch_demo.cpp \
    $(SRC)/core/clock.cpp \
    $(SRC)/core/latch.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

# =========================
#  POWER SWITCH DEMO
# =========================

$(BIN)/power_switch_demo: \
    $(SRC)/demos/power_switch_demo.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

# =========================
#  KJØRING
# =========================
run-%: $(BIN)/%
	@echo "▶️ Kjører $*"
	@./$(BIN)/$*

# =========================
#  RYDD OPP
# =========================
clean:
	rm -rf $(OBJ) $(BIN)
