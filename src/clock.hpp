#pragma once
#include <string>

enum class Mode { STEP, RUN, HALT };

enum class ClockEdge {
    None,
    Rising,
    Falling
};

class Clock {
private:
    bool state = false;
    bool lastState = false;
    double frequency = 1.0; // ← NY (valgfritt, for fremtidig bruk)

public:
    Clock() = default;                   // ← Behold default
    Clock(double hz) : frequency(hz) {}  // ← Legg til dette

    ClockEdge tick();                    // én puls
    bool isHigh() const { return state; }
    bool isRisingEdge() const;       // LOW → HIGH
    bool isFallingEdge() const;      // HIGH → LOW
    std::string visual() const;      // "▮" eller "_"
};
