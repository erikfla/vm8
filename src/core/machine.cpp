// src/core/machine.cpp
#include "machine.hpp"
#include <iostream>
extern bool verbose;

Machine::Machine() {
    // ── Nettlisten: kobler porter til klokkelinjer ────────
    // Rekkefølge = koblingsskjema.

    // mainClk: utganger driver bussen
    clk_.mainClk.connect(&regPC_.output);
    clk_.mainClk.connect(&ram_.output);
    clk_.mainClk.connect(&regA_.output);
    clk_.mainClk.connect(&regIR_.output);
    clk_.mainClk.connect(&alu_.output);

    // controlClk: latch → display → snapshot → step → count → ROM
    clk_.controlClk.connect(&regMAR_.input);
    clk_.controlClk.connect(&regIR_.input);
    clk_.controlClk.connect(&regA_.input);
    clk_.controlClk.connect(&regB_.input);
    clk_.controlClk.connect(&regOUT_.input);
    clk_.controlClk.connect(&regPC_.input);
    clk_.controlClk.connect(&alu_.input);
    clk_.controlClk.connect(&ram_.input);
    clk_.controlClk.connect(&outDisplay_);
    if (debugMode_) clk_.controlClk.connect(&dbg_);
    clk_.controlClk.connect(&step_);
    clk_.controlClk.connect(&instrCounter_);
    clk_.controlClk.connect(&rom_);

    reset();
}

void Machine::reset() {
    step_.clear();
    instrCount_ = 0;
    dbg_.reset();
    clk_.reset();
    bus_.reset();

    regMAR_.setValue(0);
    regA_.setValue(0);
    regB_.setValue(0);
    regIR_.setValue(0);
    regOUT_.setValue(0);
    regPC_.setValue(0);
    outDisplay_.last = 0;  // synk med regOUT_ slik at reset ikke trigger [OUT] 0

    rom_.update();
    if (verbose) std::cout << "[Machine] Reset\n";
}

void Machine::loadProgram(const std::array<uint8_t, 16>& program) {
    ram_.load(program);
}
