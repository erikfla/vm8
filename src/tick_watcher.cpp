#include <iostream>
#include "clock.hpp"

int main() {
    Clock clk;

    std::cout << "=== KLOKKE-TICKDEMO ===\n";
    std::cout << "Trykk Enter for en pulssyklus. Skriv 'q' + Enter for å avslutte.\n";

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "q") {
            std::cout << "=== AVSLUTTER ===\n";
            break;
        }

        std::cout << "Starter én pulssyklus:\n";
        clk.tick();  // Høy → lav
        clk.tick();  // Lav → høy
    }

    return 0;
}
