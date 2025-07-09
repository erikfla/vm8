#include "clock.hpp"
#include "latch.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    Clock clk(1.0); // 1 Hz
    DLatch bit;

    bool d_input = false;

    std::cout << "=== Latch koblet til klokke ===\n";
    std::cout << "Trykk [0]/[1] for å endre D-input\n";
    std::cout << "Trykk [Enter] for å ta et klokkeslag\n";
    std::cout << "Trykk q for å avslutte\n\n";

    std::string cmd;
    while (true) {
        std::cout << "[clk: " << (clk.isHigh() ? "↑" : "_")
                  << " | D: " << d_input
                  << " | Q: " << bit.output()
                  << "] > ";

        std::getline(std::cin, cmd);
        if (cmd == "q") break;
        if (cmd == "0") d_input = false;
        if (cmd == "1") d_input = true;

        clk.tick();

        // Enable = clock is HIGH
        bool enable = clk.isHigh();
        bit.update(d_input, enable);
    }

    return 0;
}
