// src/components/bus_alu.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include "bus_register.hpp"
#include <cstdint>

// BusALU – 8-bit adderer/subtraktor med to porter.
//
// Analogi: To 74LS283 + XOR-porter fra Ben Eater. Kombinatorisk logikk.
//
//   output – kobles til mainClk:    EO → beregn og skriv til bus
//   input  – kobles til controlClk: FI → latch carry og zero-flagg

class BusALU {
public:
    struct OutputPort : Component {
        BusALU& alu;
        explicit OutputPort(BusALU& a) : alu(a) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (!(alu.bus_.ctrl() & EO)) return;
            uint16_t r = (alu.bus_.ctrl() & SU)
                ? (uint16_t)(alu.regA_.value() - alu.regB_.value())
                : (uint16_t)(alu.regA_.value() + alu.regB_.value());
            alu.result_ = (uint8_t)r;
            alu.bus_.setData(alu.result_);
        }
    } output { *this };

    struct InputPort : Component {
        BusALU& alu;
        explicit InputPort(BusALU& a) : alu(a) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (!(alu.bus_.ctrl() & FI)) return;
            uint16_t r = (alu.bus_.ctrl() & SU)
                ? (uint16_t)(alu.regA_.value() - alu.regB_.value())
                : (uint16_t)(alu.regA_.value() + alu.regB_.value());
            alu.carry_ = (r > 0xFF);
            alu.zero_  = ((uint8_t)r == 0);
        }
    } input { *this };

    BusALU(SignalBus& bus, const BusRegister& regA, const BusRegister& regB)
        : bus_(bus), regA_(regA), regB_(regB) {}

    // Kombinatorisk: alltid A+B eller A-B, uavhengig av EO
    uint8_t compute() const {
        uint16_t r = (bus_.ctrl() & SU)
            ? (uint16_t)(regA_.value() - regB_.value())
            : (uint16_t)(regA_.value() + regB_.value());
        return (uint8_t)r;
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
