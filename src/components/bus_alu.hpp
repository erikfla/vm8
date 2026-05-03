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
//   output (mainClk):    EO → beregner A±B, legger på buss.
//                             Lagrer carry_pending og zero_pending.
//   input  (controlClk): FI → latches carry_pending/zero_pending til carry_/zero_.
//
// Viktig rekkefølge: EO (mainClk) skjer FØR FI (controlClk) i samme syklus,
// og FØR AI oppdaterer A-registeret. Slik er carry fra riktig operand.

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
            alu.result_        = (uint8_t)r;
            alu.carry_pending_ = (r > 0xFF);
            alu.zero_pending_  = ((uint8_t)r == 0);
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
            // Latch pending-verdiene – beregnet av EO tidligere i samme syklus
            alu.carry_ = alu.carry_pending_;
            alu.zero_  = alu.zero_pending_;
        }
    } input { *this };

    BusALU(SignalBus& bus, const BusRegister& regA, const BusRegister& regB)
        : bus_(bus), regA_(regA), regB_(regB) {}

    // Kombinatorisk: alltid fersk verdi av A±B (for frontend-visning)
    uint8_t compute() const {
        uint16_t r = (bus_.ctrl() & SU)
            ? (uint16_t)(regA_.value() - regB_.value())
            : (uint16_t)(regA_.value() + regB_.value());
        return (uint8_t)r;
    }

    bool flagCarry() const { return carry_; }
    bool flagZero()  const { return zero_; }
    void clearFlags()      { carry_ = zero_ = carry_pending_ = zero_pending_ = false; }

private:
    SignalBus&          bus_;
    const BusRegister&  regA_;
    const BusRegister&  regB_;
    uint8_t             result_        = 0;
    bool                carry_pending_ = false;
    bool                zero_pending_  = false;
    bool                carry_         = false;
    bool                zero_          = false;
};
