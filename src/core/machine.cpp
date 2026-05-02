// src/core/machine.cpp
#include "machine.hpp"
#include <iostream>

Machine::Machine() {
    reset();
}

void Machine::reset() {
    step_.clear();
    instrCount_ = 0;
    halted_ = false;
    clk_.reset();
    bus_.reset();

    regMAR_.setValue(0);
    regA_.setValue(0);
    regB_.setValue(0);
    regIR_.setValue(0);
    regOUT_.setValue(0);
    regPC_.setValue(0);

    std::cout << "[Machine] Reset\n";
}

void Machine::tick() {
    if (halted_) return;

    Edge edge = clk_.tick();
    bus_.set("CLK", clk_.isHigh());

    if (edge == Edge::Rising)  risingEdge();
    if (edge == Edge::Falling) fallingEdge();

    // HLT ble satt på rising edge – fullfør perioden (klokken ned)
    if (halted_ && clk_.isHigh()) {
        clk_.tick();
        bus_.set("CLK", false);
    }
}

// ── Rising edge: utganger driver bussen ──────────────────
// Kontrollordet settes kombinatorisk (fra IR + step),
// deretter leser hvert komponent det selv via bus_.ctrl().

void Machine::risingEdge() {
    uint8_t     opcode = (regIR_.value() >> 4) & 0xF;
    ControlWord cw     = MICROCODE[opcode][step_.value()];
    bus_.setCtrl(cw);

    std::cout << "[↑ step=" << (int)step_.value()
              << " op=0x"  << std::hex << (int)opcode
              << " ctrl=0x" << (int)cw
              << " PC=" << std::dec << (int)regPC_.value()
              << " A="  << (int)regA_.value()
              << "]\n" << std::flush;

    // Alle utganger er aktive på rising edge:
    regPC_.onRisingEdge();   // CO  – PC → bus
    ram_.onRisingEdge();     // RO  – ram[MAR] → bus
    regA_.onRisingEdge();    // AO  – A → bus
    regIR_.onRisingEdge();   // IO  – IR[3:0] → bus
    alu_.onRisingEdge();     // EO  – A ± B → bus

    // HLT sjekkes etter at alle har fått snakke
    if (cw & HLT) {
        halted_ = true;
        bus_.set("HLT", true);
        std::cout << "[HLT] Maskinen stoppet\n";
    }
}

// ── Falling edge: innganger latcher + step teller opp ────
// Step-telleren bruker invertert klokke (falling edge),
// akkurat som 74LS163 hos Ben Eater.

void Machine::fallingEdge() {
    // Alle innganger latcher fra bussen:
    regMAR_.onFallingEdge();  // MI
    regIR_.onFallingEdge();   // II  (ny instruksjon lastes hit)
    regA_.onFallingEdge();    // AI
    regB_.onFallingEdge();    // BI
    regOUT_.onFallingEdge();  // OI
    regPC_.onFallingEdge();   // CE / J
    alu_.onFallingEdge();     // FI

    if (bus_.ctrl() & OI)
        std::cout << "[OUT] " << std::dec << (int)regOUT_.value() << "\n";

    // ── Step-teller (invertert klokke = falling edge) ─────
    // StepCounter teller alltid – akkurat som 74LS163.
    // HLT stopper klokken, ikke telleren.
    bool wasT5 = (step_.value() == MAX_STEPS - 1);
    step_.onFallingEdge();
    if (wasT5) instrCount_++;  // ny instruksjon starter
}

void Machine::loadProgram(const std::array<uint8_t, 16>& program) {
    ram_.load(program);
}
