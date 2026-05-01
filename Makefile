CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Irc/core -Isrc/components

SRC = src/main.cpp \
      src/core/clock.cpp \
      src/core/signal_bus.cpp \
      src/core/machine.cpp \
      src/components/dflipflop.cpp \
      src/components/register8.cpp \
      src/components/ram.cpp

vm8: $(SRC)
	$(CXX) $(CXXFLAGS) -o vm8 $(SRC)

clean:
	rm -f vm8
