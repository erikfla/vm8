#pragma once
#include <string>

enum class Mode { STEP, RUN, HALT };

enum class ClockEdge {
    None,
    Rising,
    Falling
};

// To-fase-klokke: φ1 når signalet er høyt, φ2 når det er lavt
enum class Phase {
    Phi1, // HIGH
    Phi2  // LOW
};

class Clock {
private:
    bool state = false;       // nåværende nivå (LOW = false, HIGH = true)
    bool lastState = false;   // for edge-deteksjon
    double frequency = 1.0;   // evt. for senere bruk

public:
    Clock() = default;
    explicit Clock(double hz) : frequency(hz) {}

    // Generer ett klokke-"tick" (veksler mellom HIGH og LOW)
    ClockEdge tick();

    // Status
    bool isHigh() const;
    bool isRisingEdge() const;
    bool isFallingEdge() const;
    std::string visual() const;  // "▮" for HIGH, "_" for LOW

    // Hvilken halvsyklus er vi i? (φ1 eller φ2)
    Phase phase() const;

    void reset();
};
