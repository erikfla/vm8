// src/core/machine.cpp
#include "machine.hpp"
#include <iostream>

Machine::Machine() {
    reset();
}

void Machine::reset() {
    a = b = pc = ir = out = 0;
    halted = false;
    clk.reset();
    std::cout << "[Machine] Reset\n";
}

void Machine::tick() {
    if (halted) return;
    Edge edge = clk.tick();
    bus.set("CLK", clk.isHigh());
    handleEdge(edge);
}

void Machine::handleEdge(Edge edge) {
    if (edge == Edge::Rising) {
        fetchAndExecute();
    }
}

void Machine::fetchAndExecute() {
    // Hent instruksjon fra RAM
    ir = ram[pc];
    uint8_t opcode  = (ir >> 4) & 0xF;
    uint8_t operand = ir & 0xF;

    switch (opcode) {
        case 0x0:  // NOP
            break;
        case 0x1:  // LDA – last A fra RAM
            a = ram[operand];
            break;
        case 0x2:  // ADD – A = A + RAM[operand]
            b = ram[operand];
            a = a + b;
            break;
        case 0x3:  // SUB – A = A - RAM[operand]
            b = ram[operand];
            a = a - b;
            break;
        case 0x4:  // OUT – vis A
            out = a;
            std::cout << "[OUT] " << (int)out << "\n";
            break;
        case 0xF:  // HLT
            halted = true;
            bus.set("HLT", true);
            std::cout << "[HLT] Maskinen stoppet\n";
            return;
    }

// JSON til stdout (for bridge.py)
std::cout << "{\"clock\":\""  << (clk.isHigh() ? "HIGH" : "LOW")
          << "\",\"pc\":"     << (int)pc
          << ",\"ir\":"       << (int)ir
          << ",\"a\":"        << (int)a
          << ",\"out\":"      << (int)out
          << ",\"halted\":"   << (halted ? "true" : "false")
          << "}\n" << std::flush;

    pc++;
    if (pc >= 16) pc = 0;

    bus.setData(a);
    bus.set("CLK", clk.isHigh());
}

void Machine::loadProgram(const std::array<uint8_t, 16>& program) {
    ram = program;
}
