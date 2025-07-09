#include "clock.hpp"
#include <iostream>

void Clock::tick() {
    lastState = state;
    state = !state;

    std::cout << "CLK: " << visual();
    if (isRisingEdge())  std::cout << "  ↗ RISING edge\n";
    if (isFallingEdge()) std::cout << "  ↘ FALLING edge\n";
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
