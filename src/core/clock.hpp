// src/core/clock.hpp
#pragma once
#include <string>

enum class ClockEdge {
    None,
    Rising,
    Falling
};

class Clock {
private:
    bool state = false;
    bool lastState = false;
    double frequency = 1.0; // evt. til senere bruk

public:
    Clock() = default;
    Clock(double hz) : frequency(hz) {}

    // Sett klokka tilbake til definert starttilstand
    void reset();

    // Én halvsyklus: toggle state og returner edge-typen
    ClockEdge tick();

    bool isHigh() const { return state; }

    bool isRisingEdge() const { return (!lastState && state); }   // LOW → HIGH
    bool isFallingEdge() const { return (lastState && !state); }  // HIGH → LOW

    std::string visual() const { return state ? "▮" : "_"; }
};
