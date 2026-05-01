// src/core/clock.cpp
#include "clock.hpp"

Edge Clock::tick() {
    state = !state;
    return state ? Edge::Rising : Edge::Falling;
}

bool Clock::get(const std::string& pin) const {
    if (pin == "CLK") return state;
    return false;
}

void Clock::reset() {
    state = false;
}
