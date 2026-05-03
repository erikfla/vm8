CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc -Isrc/core -Isrc/components

SRC = src/main.cpp \
      src/core/machine.cpp

HTTPLIB = src/httplib.h

.DEFAULT_GOAL := vm8

vm8: $(HTTPLIB) $(SRC)
	$(CXX) $(CXXFLAGS) -o vm8 $(SRC) -lpthread

$(HTTPLIB):
	@echo "→ Laster ned cpp-httplib..."
	curl -sL https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o $@

clean:
	rm -f vm8
