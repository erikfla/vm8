// src/components/prog_counter.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include <cstdint>

// ProgCounter – 4-bit programteller med to porter.
//
// Analogi: 74LS163 fra Ben Eater, koblet til hoved-CLK.
//
//   output – kobles til mainClk:    CO → skriv PC til bus
//   input  – kobles til controlClk: CE → inkrementer, J → hopp

class ProgCounter {
public:
    struct OutputPort : Component {
        ProgCounter& pc;
        explicit OutputPort(ProgCounter& p) : pc(p) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (pc.bus_.ctrl() & CO)
                pc.bus_.setData(pc.val_);
        }
    } output { *this };

    struct InputPort : Component {
        ProgCounter& pc;
        explicit InputPort(ProgCounter& p) : pc(p) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (pc.bus_.ctrl() & J)
                pc.val_ = pc.bus_.getData() & 0x0F;
            else if (pc.bus_.ctrl() & CE)
                pc.val_ = (pc.val_ + 1) & 0x0F;
        }
    } input { *this };

    explicit ProgCounter(SignalBus& bus) : bus_(bus) {}

    uint8_t value()       const { return val_; }
    void    setValue(uint8_t v) { val_ = v & 0x0F; }

private:
    SignalBus& bus_;
    uint8_t    val_ = 0;
};
