// src/components/dflipflop.hpp
#pragma once
#include "../core/component.hpp"

// D-flipflop – den mest grunnleggende hukommelsescellen.
//
// På rising edge lagres verdien på D-pinnen i Q.
// Dette tilsvarer en 74LS74-krets.
//
// Pinner:
//   Inn:  D
//   Ut:   Q, Qn (invertert Q)

class DFlipFlop : public Component {
public:
    void set(const std::string& pin, bool value) override;
    bool get(const std::string& pin) const override;
    void onRisingEdge() override;

private:
    bool D  = false;
    bool Q  = false;
};
