// src/main.cpp
#include "core/machine.hpp"
#include <iostream>
#include <array>

int main() {
    Machine m;

    // Enkelt testprogram – inspirert av Ben Eater:
    // LDA 14    – last verdien på adresse 14 inn i A
    // ADD 15    – legg til verdien på adresse 15
    // OUT       – vis resultatet
    // HLT       – stopp
    std::array<uint8_t, 16> program = {
        0x1E,  // LDA 14
        0x2F,  // ADD 15
        0x40,  // OUT
        0xF0,  // HLT
        0,0,0,0,0,0,0,0,0,0,
        28,    // adresse 14: 28
        14     // adresse 15: 14
    };

    m.loadProgram(program);

    std::cout << "=== VM8 starter ===\n";

    for (int i = 0; i < 20 && !m.isHalted(); i++) {
        m.tick();
    }

    std::cout << "=== VM8 ferdig ===\n";
    return 0;
}
