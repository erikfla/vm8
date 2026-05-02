// src/components/bus_register.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include <cstdint>

// BusRegister – 8-bit register direkte koblet til SignalBus.
//
// Analogi: 74LS173 (register) koblet til bussen med OE og LOAD styrt
// av kontrollordet fra mikrokodetabellen.
//
// Konstruktør-parametere:
//   bus     – felles databus
//   outBit  – kontrollbit som aktiverer output (f.eks. AO).
//             Sett til NONE hvis registeret aldri driver bussen.
//   inBit   – kontrollbit som aktiverer input (f.eks. AI).
//             Sett til NONE hvis registeret aldri laster fra bussen.
//
// Sekvens:
//   Rising edge  → hvis outBit er satt i ctrl: skriv val til bus.data
//   Falling edge → hvis inBit  er satt i ctrl: les bus.data inn i val

class BusRegister : public Component {
public:
    // outMask: maske på val_ som legges på bussen (0xFF = hele byte,
    //          0x0F = laveste nibble – brukes av IR for IO/operand)
    BusRegister(SignalBus& bus, ControlWord outBit, ControlWord inBit,
                uint8_t outMask = 0xFF)
        : bus_(bus), outBit_(outBit), inBit_(inBit), outMask_(outMask) {}

    // Component-grensesnitt (bussen er ledninger – pin-grensesnitt ubrukt)
    void set(const std::string&, bool) override {}
    bool get(const std::string&)  const override { return false; }

    // Rising edge: driver bussen hvis kontrollbit er aktiv
    void onRisingEdge() override {
        if (outBit_ && (bus_.ctrl() & outBit_))
            bus_.setData(val_ & outMask_);
    }

    // Falling edge: latcher fra bussen hvis kontrollbit er aktiv
    void onFallingEdge() override {
        if (inBit_ && (bus_.ctrl() & inBit_))
            val_ = bus_.getData();
    }

    // Lese ut verdien (for JSON og til ALU)
    uint8_t value() const { return val_; }

    // Direkte sette verdi (reset / load program)
    void setValue(uint8_t v) { val_ = v; }

private:
    SignalBus&  bus_;
    ControlWord outBit_;
    ControlWord inBit_;
    uint8_t     outMask_;
    uint8_t     val_ = 0;
};
