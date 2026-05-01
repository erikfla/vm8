// src/components/register8.hpp
#pragma once
#include "../core/component.hpp"
#include "dflipflop.hpp"
#include <array>

// 8-bit register bygget av 8 D-flipflops.
//
// På rising edge lagres verdien på databussen (D0-D7) hvis LOAD er HIGH.
// Verdien kan leses ut på Q0-Q7 hvis OE (output enable) er HIGH.
//
// Dette tilsvarer to 74LS173-kretser hos Ben Eater.
//
// Pinner:
//   Inn:  D0-D7  (databus inn)
//         LOAD   (last inn verdi på rising edge)
//         OE     (output enable)
//   Ut:   Q0-Q7  (databus ut)

class Register8 : public Component {
public:
    void set(const std::string& pin, bool value) override;
    bool get(const std::string& pin) const override;
    void onRisingEdge() override;

private:
    std::array<DFlipFlop, 8> bits;
    bool load = false;
    bool oe   = false;
};
