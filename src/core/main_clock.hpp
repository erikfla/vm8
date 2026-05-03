// src/core/main_clock.hpp
#pragma once
#include "clock_signal.hpp"
#include "signal_bus.hpp"
#include "control.hpp"

// Edge – retning på siste klokkekant
enum class Edge { None, Rising, Falling };

// MainClock – klokkemodell med to utganger: CLK og /CLK.
//
// Analogi: 555-timer + 74LS04-inverter fra Ben Eaters klokke-modul.
//
//   mainClk    – hoved-CLK  (registre, PC, ALU, RAM)
//   controlClk – /CLK       (step-teller, MicrocodeROM)
//
// HLT-gating: når HLT er satt i kontrollordet, stopper tick() –
// akkurat som en AND-port mellom 555-utgangen og resten av kretsen.
//
// Ekstern kilde: kall set(bool) fra ekte hardware, serial port, etc.
// Intern kilde:  kall tick() for simulert oscillator.

class MainClock {
public:
    ClockSignal mainClk;     // CLK
    ClockSignal controlClk;  // /CLK (invertert via 74LS04)

    explicit MainClock(SignalBus& bus) : bus_(bus) {}

    // Simulert oscillator – togglar til motsatt tilstand
    Edge tick() {
        if (bus_.ctrl() & HLT) return Edge::None;  // klokkeporten stengt
        return step(!mainClk.isHigh());
    }

    // Ekstern kilde – sett tilstand direkte (ekte oscillator, etc.)
    void set(bool high) {
        if (high == mainClk.isHigh()) return;
        step(high);
    }

    bool isHigh() const { return mainClk.isHigh(); }

    void reset() {
        mainClk.fall();
        controlClk.fall();
        bus_.set("CLK", false);
    }

private:
    Edge step(bool goHigh) {
        if (goHigh) {
            mainClk.rise();
            controlClk.fall();
            bus_.set("CLK", true);
            return Edge::Rising;
        } else {
            mainClk.fall();
            controlClk.rise();
            bus_.set("CLK", false);
            return Edge::Falling;
        }
    }

    SignalBus& bus_;
};
