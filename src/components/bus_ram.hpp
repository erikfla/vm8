// src/components/bus_ram.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include "bus_register.hpp"
#include <array>
#include <cstdint>

// BusRAM – 16 x 8-bit statisk minne.
//
// Analogi: 74189 (16x4 RAM) x2 hos Ben Eater.
// MAR (Memory Address Register) er koblet direkte som adresseingang –
// vi tar det som referanse til regMAR.
//
// RO (RAM Out) – rising edge: skriv ram[mar] til bus
// RI (RAM In)  – falling edge: les bus.data og skriv til ram[mar]

class BusRAM : public Component {
public:
    BusRAM(SignalBus& bus, const BusRegister& mar)
        : bus_(bus), mar_(mar) {}

    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    void onRisingEdge() override {
        if (bus_.ctrl() & RO)
            bus_.setData(mem_[mar_.value() & 0x0F]);
    }

    void onFallingEdge() override {
        if (bus_.ctrl() & RI)
            mem_[mar_.value() & 0x0F] = bus_.getData();
    }

    // Laste inn program fra array
    void load(const std::array<uint8_t, 16>& data) { mem_ = data; }

    // Les enkeltcelle (for JSON)
    uint8_t at(uint8_t addr) const { return mem_[addr & 0x0F]; }

private:
    SignalBus&          bus_;
    const BusRegister&  mar_;
    std::array<uint8_t, 16> mem_ = {};
};
