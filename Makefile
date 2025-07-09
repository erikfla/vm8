# Kompilator og flagg
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Mapper
SRC = src
BIN = bin

# Lag bin-mappen hvis den ikke finnes
$(shell mkdir -p $(BIN))

# Kildefiler
CLOCK_SRC = $(SRC)/clock.cpp
CLOCK_HDR = $(SRC)/clock.hpp

# Programmer
KLOKKE_SRC = $(SRC)/tick_watcher.cpp
KLOKKE_BIN = $(BIN)/klokke

CPU_SRC = $(SRC)/cpu.cpp
CPU_BIN = $(BIN)/vm8

# Standardmål: bygg alt
all: $(KLOKKE_BIN) $(CPU_BIN)

# Klokke-demo
$(KLOKKE_BIN): $(CLOCK_SRC) $(KLOKKE_SRC) $(CLOCK_HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(CLOCK_SRC) $(KLOKKE_SRC)

# CPU-program (foreløpig placeholder – kan erstattes)
$(CPU_BIN): $(CPU_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(CPU_SRC)

# Kjør klokke
run-klokke: $(KLOKKE_BIN)
	./$(KLOKKE_BIN)

LATCH_SRC = $(SRC)/latch.cpp
LATCH_HDR = $(SRC)/latch.hpp
LATCH_DEMO = $(SRC)/latch_demo.cpp
LATCH_BIN = $(BIN)/latch_demo
LATCH_DEMO2 = $(SRC)/latch_demo2.cpp
LATCH_BIN2 = $(BIN)/latch_demo2

$(LATCH_BIN2): $(LATCH_SRC) $(LATCH_HDR) $(LATCH_DEMO2)
	$(CXX) $(CXXFLAGS) -o $@ $(LATCH_SRC) $(LATCH_DEMO2)

run-latch2: $(LATCH_BIN2)
	./$(LATCH_BIN2)

$(LATCH_BIN): $(LATCH_SRC) $(LATCH_HDR) $(LATCH_DEMO)
	$(CXX) $(CXXFLAGS) -o $@ $(LATCH_SRC) $(LATCH_DEMO)

run-latch: $(LATCH_BIN)
	./$(LATCH_BIN)

# Rydd opp
clean:
	rm -rf $(BIN)

.PHONY: all clean run-klokke
