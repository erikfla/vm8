// src/components/ram.cpp
#include "ram.hpp"

void RAM::set(const std::string& pin, bool value) {
    if (pin == "WE") { we = value; return; }
    if (pin == "OE") { oe = value; return; }

    // Adressepinner A0-A3
    if (pin.size() == 2 && pin[0] == 'A') {
        int i = pin[1] - '0';
        if (i >= 0 && i < 4) {
            if (value) address |=  (1 << i);
            else       address &= ~(1 << i);
        }
        return;
    }

    // Datapinner D0-D7
    if (pin.size() == 2 && pin[0] == 'D') {
        int i = pin[1] - '0';
        if (i >= 0 && i < 8) {
            if (value) dataIn |=  (1 << i);
            else       dataIn &= ~(1 << i);
        }
    }
}

bool RAM::get(const std::string& pin) const {
    if (!oe) return false;

    if (pin.size() == 2 && pin[0] == 'Q') {
        int i = pin[1] - '0';
        if (i >= 0 && i < 8) {
            return (mem[address] >> i) & 1;
        }
    }
    return false;
}

void RAM::onRisingEdge() {
    if (we) mem[address] = dataIn;
}

void RAM::load(const std::array<uint8_t, 16>& program) {
    mem = program;
}
