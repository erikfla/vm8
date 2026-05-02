// src/components/bus_alu.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include "bus_register.hpp"
#include <cstdint>

// BusALU – 8-bit adderer/subtraktor.
//
// Analogi: To 74LS283 (4-bit adderer) pluss XOR-porter for subtraksjon,
// kombinatorisk – ingen intern klokkeingang.
//
// Innganger er hardkoblet til regA og regB (som faktiske ledninger).
// EO (ALU Out) gjør at resultatet legges på bussen på rising edge.
// SU (Subtract) velger A-B i stedet for A+B.
// FI (Flags In) lagrer carry og zero på falling edge.

class BusALU : public Component {
public:
    BusALU(SignalBus& bus, const BusRegister& regA, const BusRegister& regB)
        : bus_(bus), regA_(regA), regB_(regB) {}

    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    // Rising edge: EO → beregn og skriv til bus
    void onRisingEdge() override {
        if (!(bus_.ctrl() & EO)) return;

        uint8_t a = regA_.value();
        uint8_t b = regB_.value();
        uint16_t result = (bus_.ctrl() & SU) ? (uint16_t)(a - b)
                                              : (uint16_t)(a + b);
        result_ = (uint8_t)result;
        bus_.setData(result_);
    }

    // Falling edge: FI → latch flagg
    void onFallingEdge() override {
        if (!(bus_.ctrl() & FI)) return;
        // Vi bruker verdien vi beregnet på rising edge
        carry_ = (regA_.value() + regB_.value()) > 0xFF;  // re-beregn for FI uten EO
        zero_  = (result_ == 0);
        if (bus_.ctrl() & EO) {
            // EO|FI – resultatet er allerede korrekt fra rising edge
            uint16_t r = (bus_.ctrl() & SU)
                ? (uint16_t)(regA_.value() - regB_.value())
                : (uint16_t)(regA_.value() + regB_.value());
            carry_ = (r > 0xFF);
            zero_  = ((uint8_t)r == 0);
        }
    }

    bool flagCarry() const { return carry_; }
    bool flagZero()  const { return zero_;  }

private:
    SignalBus&          bus_;
    const BusRegister&  regA_;
    const BusRegister&  regB_;
    uint8_t             result_ = 0;
    bool                carry_  = false;
    bool                zero_   = false;
};
