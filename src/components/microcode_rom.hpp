// src/components/microcode_rom.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/microcode.hpp"
#include "bus_register.hpp"
#include "step_counter.hpp"

// MicrocodeROM – kombinatorisk kontrollord-generator.
//
// Analogi: Ben Eaters to EEPROM-brikker (AT28C16).
// Kobles til MainClock::controlClk (etter step_) i Machine-konstruktøren.
// Oppdateres automatisk når /CLK stiger (step++ er ferdig).
//
// update() kan kalles eksplisitt etter reset eller restore.

class MicrocodeROM : public Component {
public:
    MicrocodeROM(SignalBus& bus, BusRegister& ir, StepCounter& step)
        : bus_(bus), ir_(ir), step_(step) {}

    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    void update() {
        uint8_t opcode = (ir_.value() >> 4) & 0xF;
        bus_.setCtrl(MICROCODE[opcode][step_.value()]);
    }

    // Kobles til controlClk – oppdaterer etter step++ er ferdig
    void onRisingEdge() override { update(); }

private:
    SignalBus&   bus_;
    BusRegister& ir_;
    StepCounter& step_;
};
