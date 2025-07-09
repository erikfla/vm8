#include "latch.hpp"
#include <iostream>
#include <string>

int main() {
    DLatch bit;
    std::string cmd;
    bool input = false;

    std::cout << "=== 1-bit Celle Demo ===\n";
    std::cout << "Kommandoer:\n";
    std::cout << "  0   → sett inn 0\n";
    std::cout << "  1   → sett inn 1\n";
    std::cout << "  t   → toggle klokke\n";
    std::cout << "  r   → reset cella\n";
    std::cout << "  q   → avslutt\n\n";

    while (true) {
        std::cout << "[verdi: " << bit.output() << "] > ";
        std::getline(std::cin, cmd);

        if (cmd == "q") break;
        else if (cmd == "0") input = false;
        else if (cmd == "1") input = true;
        else if (cmd == "t") {
            bit.update(input, true);   // Klokke høy
            bit.update(input, false);  // Klokke lav (slipp)
        }
        else if (cmd == "r") bit.reset();
    }

    std::cout << "=== Avslutter ===\n";
    return 0;
}
