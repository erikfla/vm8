// src/components/step_counter.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/microcode.hpp"
#include <cstdint>

// StepCounter – 3-bit mikrosteg-teller.
//
// Analogi: 74LS163 (4-bit binær teller) hos Ben Eater,
// koblet til invertert klokke slik at den teller på falling edge.
//
// Telleren teller 0 → MAX_STEPS-1 og resetter automatisk.
// Den vet ingenting om instruksjoner eller mikrokodetabellen –
// den bare teller, akkurat som ekte hardware.
//
// CLR (reset) styres utenfra – f.eks. ved maskin-reset.

class StepCounter : public Component {
public:
    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    // Falling edge = invertert klokke, som 74LS163 hos Ben Eater
    void onFallingEdge() override {
        count_ = (count_ + 1) % MAX_STEPS;
    }

    uint8_t value() const { return count_; }

    // CLR-inngang – resetter telleren til 0
    void clear() { count_ = 0; }

private:
    uint8_t count_ = 0;
};
