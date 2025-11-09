#include "clock.hpp"
#include <iostream>

ClockEdge Clock::tick() {
    state = !state;
    if (state) {
        std::cout << "CLK: ▮  ↗ RISING edge\n";
        return ClockEdge::Rising;
    } else {
        std::cout << "CLK: _  ↘ FALLING edge\n";
        return ClockEdge::Falling;
    }
}

bool Clock::isRisingEdge() const {
    return !lastState && state;
}

bool Clock::isFallingEdge() const {
    return lastState && !state;
}

std::string Clock::visual() const {
    return state ? "▮" : "_";
}
