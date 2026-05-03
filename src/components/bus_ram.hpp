// src/components/bus_ram.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include "bus_register.hpp"
#include <array>
#include <cstdint>

// BusRAM – 16 x 8-bit statisk minne med to porter.
//
// Analogi: 74189 x2 fra Ben Eater. Adressert av MAR.
//
//   output – kobles til mainClk:    RO → skriv ram[mar] til bus
//   input  – kobles til controlClk: RI → skriv bus til ram[mar]

class BusRAM {
public:
    struct OutputPort : Component {
        BusRAM& ram;
        explicit OutputPort(BusRAM& r) : ram(r) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (ram.bus_.ctrl() & RO)
                ram.bus_.setData(ram.mem_[ram.mar_.value() & 0x0F]);
        }
    } output { *this };

    struct InputPort : Component {
        BusRAM& ram;
        explicit InputPort(BusRAM& r) : ram(r) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (ram.bus_.ctrl() & RI)
                ram.mem_[ram.mar_.value() & 0x0F] = ram.bus_.getData();
        }
    } input { *this };

    BusRAM(SignalBus& bus, const BusRegister& mar)
        : bus_(bus), mar_(mar) {}

    void    load(const std::array<uint8_t, 16>& data) { mem_ = data; }
    uint8_t at(uint8_t addr) const { return mem_[addr & 0x0F]; }

private:
    SignalBus&          bus_;
    const BusRegister&  mar_;
    std::array<uint8_t, 16> mem_ = {};
};
