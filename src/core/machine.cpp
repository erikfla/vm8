// src/core/machine.cpp
#include "machine.hpp"
#include <iostream>

Machine::Machine() {
    reset();
}

void Machine::reset() {
    halted = false;
    clk.reset();
    lastEdge = ClockEdge::None;

    // Initielle signaler
    signalBus.set("CLK", clk.isHigh());
    signalBus.set("HLT", false);
}

void Machine::tick() {
    if (halted) {
        return;
    }

    // 1. Tikk klokka
    ClockEdge edge = clk.tick();
    lastEdge = edge;

    // 2. Oppdater clock-signalet i signalbussen
    signalBus.set("CLK", clk.isHigh());

    // 3. La maskinen reagere på denne edge'n
    handleClockEdge(edge);
}

void Machine::handleClockEdge(ClockEdge edge) {
    // Her skal vi etter hvert:
    //  - kjøre mikrokode på rising edge
    //  - oppdatere registre, bus, ALU, osv.

    if (edge == ClockEdge::Rising) {
        // Placeholder for fremtidig CPU-logikk
        std::cout << "[Machine] Rising edge\n";
    } else if (edge == ClockEdge::Falling) {
        std::cout << "[Machine] Falling edge\n";
    }

    // Foreløpig stopper vi aldri:
    signalBus.set("HLT", false);
    halted = false;
}
