// src/core/machine.hpp
#pragma once
#include "clock.hpp"
#include "signal_bus.hpp"

class Machine {
public:
    Machine();

    // Sett maskinen i en definert starttilstand
    void reset();

    // Én system-tick:
    //  - tikk klokka
    //  - reager på edge
    void tick();

    // Har maskinen stoppet? (HLT-signal)
    bool isHalted() const { return halted; }

    // Tilgang for front-end / demo
    const Clock& clock() const { return clk; }
    const SignalBus& signals() const { return signalBus; }

private:
    Clock clk;
    SignalBus signalBus;
    bool halted = false;
    ClockEdge lastEdge = ClockEdge::None;

    // Internt: hva skjer når vi får en klokke-edge?
    void handleClockEdge(ClockEdge edge);
};
