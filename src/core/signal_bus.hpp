// src/core/signal_bus.hpp
#pragma once
#include "control.hpp"
#include <string>
#include <unordered_map>
#include <cstdint>

// SignalBus er de fysiske ledningene – databus + kontrollsignaler.
//
// Tenk på den som breadboardet hos Ben Eater:
//   - data  = den 8-bit parallellbussen alle registre er koblet til
//   - ctrl  = kontrollordets aktive bitmønster (fra mikrokodetabellen)
//
// Alle komponenter sjekker selv om de skal snakke på bussen
// ved å lese ctrl via bus.ctrl() i onRisingEdge / onFallingEdge.

class SignalBus {
public:
    // ── Kontrollord ───────────────────────────────────────
    // Settes av Machine ved starten av hvert mikrosteg.
    void        setCtrl(ControlWord cw) { ctrl_ = cw; }
    ControlWord ctrl()            const { return ctrl_; }

    // ── 8-bit databus ─────────────────────────────────────
    void    setData(uint8_t value) { data_ = value; }
    uint8_t getData()        const { return data_; }

    // ── Navngitte signaler (CLK, HLT, etc.) ───────────────
    void set(const std::string& name, bool value) { signals_[name] = value; }
    bool get(const std::string& name) const {
        auto it = signals_.find(name);
        return (it != signals_.end()) ? it->second : false;
    }

    // Alle signaler – for JSON
    const std::unordered_map<std::string, bool>& all() const { return signals_; }

    void reset() { data_ = 0; ctrl_ = NONE; signals_.clear(); }

private:
    ControlWord ctrl_ = NONE;
    uint8_t     data_ = 0;
    std::unordered_map<std::string, bool> signals_;
};
