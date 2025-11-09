#include "latch.hpp"
#include <iostream>
#include <string>
#include <iomanip>

int main() {
    DLatch bit;
    bool input = false;
    bool enable = false;
    bool prev_output = bit.output();
    std::string cmd;

    std::cout << "=== 1-bit Celle Demo v2 ===\n\n";
    std::cout << "Kommandoer:\n";
    std::cout << "  0   → sett input = 0\n";
    std::cout << "  1   → sett input = 1\n";
    std::cout << "  t   → klokkeimpuls (enable høy → lav)\n";
    std::cout << "  r   → reset cella\n";
    std::cout << "  q   → avslutt\n\n";

    while (true) {
        bool output = bit.output();
        bool changed = (output != prev_output);

        std::cout << std::setw(4) << "IN: " << input
                  << "   CLK: " << (enable ? "↑" : "_")
                  << "   OUT: " << output
                  << (changed ? "  <== endret" : "")
                  << "\n";

        std::cout << "> ";
        std::getline(std::cin, cmd);

        prev_output = output;

        if (cmd == "q") break;
        else if (cmd == "0") input = false;
        else if (cmd == "1") input = true;
        else if (cmd == "t") {
            enable = true;
            bit.update(input, enable);   // klokke høy
            enable = false;
            bit.update(input, enable);   // klokke lav
        }
        else if (cmd == "r") bit.reset();
        else std::cout << "Ukjent kommando\n";
    }

    std::cout << "\n=== Avslutter ===\n";
    return 0;
}
