// src/core/machine.cpp
#include "machine.hpp"
#include <iostream>

Machine::Machine() {
    reset();
}

void Machine::reset() {
    // Maskinen er ikke haltet etter reset
    halted = false;

    // Klokka tilbake til definert starttilstand (LOW)
    clk.reset();
    lastEdge = ClockEdge::None;

    // Initielle signaler på bussen
    signalBus.set("CLK", clk.isHigh());  // blir false etter reset
    signalBus.set("HLT", false);         // CPU er ikke haltet ennå

    std::cout << "[Machine] Reset utført.\n";
}

void Machine::tick() {
    if (halted) {
        return; // intern HLT: maskinen gjør ikke mer
    }

    // 1. Tikk klokka
    ClockEdge edge = clk.tick();
    lastEdge = edge;

    // 2. Oppdater CLK-signal på bussen
    signalBus.set("CLK", clk.isHigh());

    // 3. La maskinen reagere på denne edge'n
    handleClockEdge(edge);
}

void Machine::handleClockEdge(ClockEdge edge) {
    // Her putter vi senere mikrokode, registeroppdatering osv.

    if (edge == ClockEdge::Rising) {
        std::cout << "[Machine] Rising edge\n";
    } else if (edge == ClockEdge::Falling) {
        std::cout << "[Machine] Falling edge\n";
    }

    // Foreløpig: HLT alltid false, maskinen stopper aldri av seg selv
    signalBus.set("HLT", false);
    halted = false;
}
