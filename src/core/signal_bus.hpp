// src/core/signal_bus.hpp
#pragma once
#include "control.hpp"
#include <unordered_map>
#include <string>
#include <cstdint>

// SignalBus – den delte databussen + kontrollord.
//
// Analogt med Ben Eaters W-bus (breadboard):
//   setCtrl() → kontrollord fra mikrokode-ROM
//   ctrl()    → leses av alle komponenter på rising/falling edge
//   setData() → data på bussen (8-bit)
//   getData() → leses av alle med inngang-latch
//
// set/get brukes for named boolske signaler (CLK, HLT, …)

class SignalBus {
public:
    void        setCtrl(ControlWord cw)          { ctrl_ = cw; }
    ControlWord ctrl()                     const { return ctrl_; }

    void    setData(uint8_t value)               { data_ = value; }
    uint8_t getData()                      const { return data_; }

    void set(const std::string& name, bool value){ signals_[name] = value; }
    bool get(const std::string& name)      const {
        auto it = signals_.find(name);
        return it != signals_.end() && it->second;
    }

    void reset() { data_ = 0; ctrl_ = NONE; signals_.clear(); }

private:
    ControlWord ctrl_ = NONE;
    uint8_t     data_ = 0;
    std::unordered_map<std::string, bool> signals_;
};
