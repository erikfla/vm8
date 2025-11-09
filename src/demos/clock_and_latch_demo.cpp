#include "clock.hpp"
#include "latch.hpp"
#include <iostream>

int main() {
    std::cout << "=== CLOCK OG LATCH DEMO ===\n\n";

    Clock clk;
    DLatch latch;

    bool D = false;
    bool EN = false;

    std::cout << "[1] Initielt: D = " << D << ", EN = " << EN << "\n";
    std::cout << "Latch output: " << latch.output() << "\n\n";

    std::cout << "[2] Sett D = 1 og EN = 1 (aktivere skriving til latch)\n";
    D = true;
    EN = true;
    latch.update(D, EN);
    std::cout << "Latch output: " << latch.output() << "\n\n";

    std::cout << "[3] Sett EN = 0 (låser verdien i latch)\n";
    EN = false;
    latch.update(D, EN);
    std::cout << "Latch output (skal fortsatt være 1): " << latch.output() << "\n\n";

    std::cout << "[4] Nå kobler vi klokka til latch via en simulert rising edge...\n";
    std::cout << "Vi simulerer D = 0 og clock-edge.\n";
    D = false;
    for (int i = 0; i < 3; ++i) {
        ClockEdge edge = clk.tick(); // simulerer rising/falling
        if (edge == ClockEdge::Rising) {
            std::cout << "⏫ RISING edge: oppdaterer latch med D = " << D << "\n";
            latch.update(D, true); // enable = true kun på rising edge
        } else {
            latch.update(D, false); // disable på falling edge
        }
        std::cout << "Latch output: " << latch.output() << "\n\n";
    }

    std::cout << "=== DEMO FERDIG ===\n";
    return 0;
}
