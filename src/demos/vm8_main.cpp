// src/demos/vm8_main.cpp
#include "machine.hpp"
#include <iostream>
#include <string>

int main() {
    Machine m;

    std::cout << "=== VM8 prototype ===\n";
    std::cout << "Trykk Enter for å tikke klokka.\n";
    std::cout << "Skriv 'q' + Enter for å avslutte.\n\n";

    while (true) {
        bool clk = m.clock().isHigh();
        bool hlt = m.signals().get("HLT");

        std::cout << "[CLK=" << (clk ? '1' : '0')
                  << ", HLT=" << (hlt ? '1' : '0') << "] > ";

        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (!line.empty() && (line[0] == 'q' || line[0] == 'Q')) {
            break;
        }

        m.tick();
    }

    std::cout << "\n=== VM8 avsluttet ===\n";
    return 0;
}
