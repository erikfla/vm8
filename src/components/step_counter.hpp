// src/components/step_counter.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/microcode.hpp"
#include <cstdint>

// StepCounter – 3-bit mikrosteg-teller.
//
// Analogi: 74LS163 koblet til /CLK (controlClk) via 74LS04.
// Ser en stigende kant akkurat idet hoved-CLK faller.
// Kobles til MainClock::controlClk i Machine-konstruktøren.

class StepCounter : public Component {
public:
    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    // Rising edge av /CLK (= falling edge av hoved-CLK)
    void onRisingEdge() override {
        count_ = (count_ + 1) % MAX_STEPS;
    }

    uint8_t value() const { return count_; }
    void    clear()       { count_ = 0; }
    void    setValue(uint8_t v) { count_ = v % MAX_STEPS; }

private:
    uint8_t count_ = 0;
};
