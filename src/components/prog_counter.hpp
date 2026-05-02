// src/components/prog_counter.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include <cstdint>

// ProgCounter – 4-bit programteller.
//
// Analogi: 74LS163 (4-bit binær teller) koblet til bussen.
// Ben Eater bruker én slik krets for PC.
//
// Kontrollbits:
//   CO  (Program Counter Out)    – rising edge: skriv PC til bus
//   CE  (Counter Enable)         – falling edge: inkrementer PC
//   J   (Jump)                   – falling edge: last inn bus.data som ny PC

class ProgCounter : public Component {
public:
    explicit ProgCounter(SignalBus& bus) : bus_(bus) {}

    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    // Rising edge: CO → skriv PC til buss
    void onRisingEdge() override {
        if (bus_.ctrl() & CO)
            bus_.setData(val_);
    }

    // Falling edge: CE → inkrementer, J → hopp (load fra bus)
    void onFallingEdge() override {
        if (bus_.ctrl() & J)
            val_ = bus_.getData() & 0x0F;   // 4-bit
        else if (bus_.ctrl() & CE)
            val_ = (val_ + 1) & 0x0F;
    }

    uint8_t value() const { return val_; }
    void    setValue(uint8_t v) { val_ = v & 0x0F; }

private:
    SignalBus& bus_;
    uint8_t    val_ = 0;
};
