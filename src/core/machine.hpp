// src/core/machine.hpp
#pragma once
#include "clock.hpp"
#include "signal_bus.hpp"

class Machine {
public:
    Machine();

    void reset();
    void tick();

    bool isHalted() const { return halted; }

    const Clock& clock() const { return clk; }
    const SignalBus& signals() const { return signalBus; }

private:
    Clock clk;
    SignalBus signalBus;
    bool halted = false;
    ClockEdge lastEdge = ClockEdge::None;

    void handleClockEdge(ClockEdge edge);
};
