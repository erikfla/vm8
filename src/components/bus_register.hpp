// src/components/bus_register.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include <cstdint>

// BusRegister – 8-bit register med to uavhengige porter.
//
// Analogi: 74LS173 fra Ben Eater.
// Én lagringscelle (val_), men to porter koblet til forskjellige klokkelinjer:
//
//   output – kobles til mainClk:    stiger → driver bussen (hvis outBit aktiv)
//   input  – kobles til controlClk: stiger → latcher fra bussen (hvis inBit aktiv)
//
// Begge porter deler val_. Data er det samme uansett retning.

class BusRegister {
public:
    struct OutputPort : Component {
        BusRegister& reg;
        explicit OutputPort(BusRegister& r) : reg(r) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (reg.outBit_ && (reg.bus_.ctrl() & reg.outBit_))
                reg.bus_.setData(reg.val_ & reg.outMask_);
        }
    } output { *this };

    struct InputPort : Component {
        BusRegister& reg;
        explicit InputPort(BusRegister& r) : reg(r) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (reg.inBit_ && (reg.bus_.ctrl() & reg.inBit_))
                reg.val_ = reg.bus_.getData();
        }
    } input { *this };

    BusRegister(SignalBus& bus, ControlWord outBit, ControlWord inBit,
                uint8_t outMask = 0xFF)
        : bus_(bus), outBit_(outBit), inBit_(inBit), outMask_(outMask) {}

    uint8_t value()       const { return val_; }
    void    setValue(uint8_t v) { val_ = v; }

private:
    SignalBus&  bus_;
    ControlWord outBit_;
    ControlWord inBit_;
    uint8_t     outMask_;
    uint8_t     val_ = 0;
};
