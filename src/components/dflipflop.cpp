// src/components/dflipflop.cpp
#include "dflipflop.hpp"

void DFlipFlop::set(const std::string& pin, bool value) {
    if (pin == "D") D = value;
}

bool DFlipFlop::get(const std::string& pin) const {
    if (pin == "Q")  return Q;
    if (pin == "Qn") return !Q;
    return false;
}

void DFlipFlop::onRisingEdge() {
    Q = D;  // husk verdien på D
}
