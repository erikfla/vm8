// src/components/register8.cpp
#include "register8.hpp"

void Register8::set(const std::string& pin, bool value) {
    if (pin == "LOAD") { load = value; return; }
    if (pin == "OE")   { oe   = value; return; }

    // D0-D7
    if (pin.size() == 2 && pin[0] == 'D') {
        int i = pin[1] - '0';
        if (i >= 0 && i < 8) bits[i].set("D", value);
    }
}

bool Register8::get(const std::string& pin) const {
    if (!oe) return false;  // ikke koblet til bussen

    if (pin.size() == 2 && pin[0] == 'Q') {
        int i = pin[1] - '0';
        if (i >= 0 && i < 8) return bits[i].get("Q");
    }
    return false;
}

void Register8::onRisingEdge() {
    if (load) {
        for (auto& ff : bits) ff.onRisingEdge();
    }
}
