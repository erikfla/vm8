// src/components/ram.hpp
#pragma once
#include "../core/component.hpp"
#include <array>
#include <cstdint>

// 16 x 8-bit RAM.
//
// Adressen settes via A0-A3 (4-bit, 16 adresser).
// Data leses/skrives via D0-D7.
//
// Pinner:
//   Inn:  A0-A3  (adresse)
//         D0-D7  (data inn)
//         WE     (write enable – skriv på rising edge)
//         OE     (output enable – kobler data ut på bussen)
//   Ut:   Q0-Q7  (data ut)

class RAM : public Component {
public:
    void set(const std::string& pin, bool value) override;
    bool get(const std::string& pin) const override;
    void onRisingEdge() override;

    // Last inn program direkte (for testing)
    void load(const std::array<uint8_t, 16>& program);

private:
    std::array<uint8_t, 16> mem = {};
    uint8_t address = 0;
    uint8_t dataIn  = 0;
    bool we = false;
    bool oe = false;
};
