// src/core/machine.cpp
#include "machine.hpp"
#include <iostream>

Machine::Machine() {
    reset();
}

void Machine::reset() {
    a = b = pc = mar = ir = out = step = 0;
    halted      = false;
    flagCarry   = false;
    flagZero    = false;
    controlWord = NONE;
    clk.reset();
    bus.set("HLT", false);
    std::cout << "[Machine] Reset\n";
}

void Machine::tick() {
    if (halted) return;

    Edge edge = clk.tick();
    bus.set("CLK", clk.isHigh());

    if (edge == Edge::Rising) {
        executeStep();
    }

    // Hvis HLT ble satt på rising edge, fullfør perioden
    if (halted && clk.isHigh()) {
        clk.tick();
        bus.set("CLK", false);
    }
}

void Machine::executeStep() {
    uint8_t opcode = (ir >> 4) & 0xF;
    controlWord    = MICROCODE[opcode][step];

    std::cout << "[STEP " << (int)step << "] "
              << "opcode=0x" << std::hex << (int)opcode
              << " ctrl=0x" << (int)controlWord
              << " PC=" << std::dec << (int)pc
              << " MAR=" << (int)mar
              << " IR=0x" << std::hex << (int)ir
              << " A=" << std::dec << (int)a
              << " bus=" << (int)bus.getData()
              << "\n" << std::flush;

    // ── Fase 1: Utganger – legg verdi på bussen ───────────
    // Kun én driver om gangen (kontrollert av maskinen)

    if (controlWord & CO) bus.setData(pc);
    if (controlWord & RO) bus.setData(ram[mar]);
    if (controlWord & AO) bus.setData(a);
    if (controlWord & IO) bus.setData(ir & 0xF);
    if (controlWord & EO) {
        uint8_t result = (controlWord & SU) ? a - b : a + b;
        bus.setData(result);
        if (controlWord & FI) {
            flagCarry = (result > 0xFF);
            flagZero  = (result == 0);
        }
    }

    // ── Fase 2: Innganger – les fra bussen ───────────────
    // Alle leser samme busverdi samtidig

    if (controlWord & MI) mar = bus.getData();
    if (controlWord & II) ir  = bus.getData();
    if (controlWord & AI) a   = bus.getData();
    if (controlWord & BI) b   = bus.getData();
    if (controlWord & OI) {
        out = bus.getData();
        std::cout << "[OUT] " << std::dec << (int)out << "\n";
    }
    if (controlWord & CE) pc = (pc + 1) & 0xF;  // 4-bit teller

    // ── HLT ──────────────────────────────────────────────
    if (controlWord & HLT) {
        halted = true;
        bus.set("HLT", true);
        std::cout << "[HLT] Maskinen stoppet\n";
        return;
    }

    // ── Neste mikrosteg ───────────────────────────────────
    step++;
    uint8_t nextOpcode = (ir >> 4) & 0xF;  // bruk oppdatert IR
    if (step >= MAX_STEPS || MICROCODE[nextOpcode][step] == NONE) {
        step = 0;
    }
}

void Machine::loadProgram(const std::array<uint8_t, 16>& program) {
    ram = program;
}
