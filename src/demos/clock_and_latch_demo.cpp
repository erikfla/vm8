#include "clock.hpp"
#include "latch.hpp"
#include <iostream>
#include <string>

static std::string phaseToString(Phase p) {
    return (p == Phase::Phi1) ? "φ1" : "φ2";
}

static std::string edgeToString(ClockEdge e) {
    switch (e) {
        case ClockEdge::Rising:  return "↑";
        case ClockEdge::Falling: return "↓";
        default:                 return " ";
    }
}

int main() {
    std::cout << "=== CLOCK + LATCH FRONT PANEL (TO-FASE KLOKKE) ===\n\n";

    DLatch bit;
    Clock clk;

    // Start definert: Q = 0
    bit.reset();

    bool d  = false;  // data inn
    bool en = false;  // enable (skriv/ikke skriv)

    std::cout << "Vi viser cycle, halvsyklus (φ1/φ2), CLK, edge, D, EN, Q.\n";
    std::cout << "Latchen oppdateres KUN på rising edge når EN = 1.\n\n";

    std::cout << "cycle | phase | CLK | edge | D | EN | Q\n";
    std::cout << "----------------------------------------\n";

    // Vi kjører noen hele klokkesykluser,
    // og endrer D/EN i begynnelsen av noen cycles
    const int NUM_CYCLES = 4;

    for (int cycle = 0; cycle < NUM_CYCLES; ++cycle) {
        // Juster D og EN ved starten av hver cycle (φ1)
        if (cycle == 0) {
            d = false;
            en = false;  // latch holder 0 (etter reset)
        } else if (cycle == 1) {
            d = true;
            en = true;   // skriv 1 inn
        } else if (cycle == 2) {
            en = false;  // hold verdien (skal holde 1)
        } else if (cycle == 3) {
            d = false;
            en = true;   // skriv 0 inn
        }

        // Hver cycle består av to halvsykluser: φ1 (HIGH) og φ2 (LOW)
        for (int half = 0; half < 2; ++half) {
            ClockEdge edge = clk.tick();
            Phase ph = clk.phase();

            // Oppdater latch KUN på rising edge
            if (edge == ClockEdge::Rising) {
                bit.update(d, en);
            }

            std::cout << "  " << cycle
                      << "    | " << phaseToString(ph)
                      << "   | " << clk.visual()
                      << "   | " << edgeToString(edge)
                      << "    | " << d
                      << " | " << en
                      << "  | " << bit.output()
                      << "\n";
        }
    }

    std::cout << "\nForklaring:\n";
    std::cout << "- φ1 (HIGH): vi lar rising edge oppdatere latchen hvis EN = 1.\n";
    std::cout << "- φ2 (LOW): vi endrer ikke latchen, men CLK går lav og forbereder neste syklus.\n";
    std::cout << "- Når EN = 0, holder Q verdien sin uansett hva D gjør.\n";

    std::cout << "\n=== DEMO FERDIG ===\n";
    return 0;
}
