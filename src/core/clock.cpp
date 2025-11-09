// src/core/clock.cpp
#include "clock.hpp"
#include <iostream>

void Clock::reset() {
    state = false;
    lastState = false;
}

ClockEdge Clock::tick() {
    // Husk forrige tilstand
    lastState = state;

    // Toggle klokka
    state = !state;

    if (state) {
        std::cout << "CLK: ▮  ↗ RISING edge\n";
        return ClockEdge::Rising;
    } else {
        std::cout << "CLK: _  ↘ FALLING edge\n";
        return ClockEdge::Falling;
    }
}
