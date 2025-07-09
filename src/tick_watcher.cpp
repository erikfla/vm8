#include "clock.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    Clock clk;
    std::string input;

    std::cout << "=== KLOKKE-TICKDEMO ===\n";
    std::cout << "Trykk Enter for å starte. 'q' + Enter for å avslutte.\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "q") break;

        std::cout << "Starter klokka...\n";
        for (int i = 0; i < 10; ++i) {
            clk.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        std::cout << "[PAUSE] – Trykk Enter for en runde til\n";
    }

    std::cout << "=== STOPP ===\n";
    return 0;
}
