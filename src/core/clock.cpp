#include "clock.hpp"
#include <iostream>

ClockEdge Clock::tick() {
    // Husk forrige tilstand for edge-deteksjon
    lastState = state;
    state = !state;

    if (state) {
        // Nå gikk vi LOW → HIGH
        std::cout << "CLK: ▮  ↗ RISING edge\n";
        return ClockEdge::Rising;
    } else {
        // Nå gikk vi HIGH → LOW
        std::cout << "CLK: _  ↘ FALLING edge\n";
        return ClockEdge::Falling;
    }
}

bool Clock::isHigh() const {
    return state;
}

bool Clock::isRisingEdge() const {
    return (!lastState && state);
}

bool Clock::isFallingEdge() const {
    return (lastState && !state);
}

std::string Clock::visual() const {
    return state ? "▮" : "_";
}

Phase Clock::phase() const {
    // Definerer φ1 som HIGH-fasen og φ2 som LOW-fasen
    return state ? Phase::Phi1 : Phase::Phi2;
}

void Clock::reset() {
    state = false;
    lastState = false;
}
