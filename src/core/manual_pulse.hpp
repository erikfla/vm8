// src/core/manual_pulse.hpp
#pragma once

#include "machine.hpp"

// En veldig enkel "monostabil": hver gang du kaller singlePulse(),
// kjører den EN FULL klokkeperiode: RISING + FALLING.
class ManualPulseGenerator {
public:
    explicit ManualPulseGenerator(Machine& m)
        : machine(m) {}

    void singlePulse() {
        // Forutsetning: Clock::reset() setter state = LOW.
        // Da blir første tick RISING, andre FALLING.
        // Selv om vi ikke kjenner starttilstand, er dette alltid én hel periode.
        machine.tick(); // første edge (f.eks. RISING)
        machine.tick(); // andre edge (FALLING)
    }

private:
    Machine& machine;
};
