// src/demos/vm8_main.cpp
#include "machine.hpp"
#include "power_switch.hpp"
#include "manual_pulse.hpp"

#include <iostream>
#include <thread>
#include <chrono>

enum class RunMode {
    MANUAL,  // klokka går bare når vi stepper
    AUTO     // klokka tikker kontinuerlig (per kommando-loop)
};

int main() {
    Machine m;
    PowerSwitch power;
    ManualPulseGenerator stepper(m);  // ← NY!

    RunMode mode = RunMode::MANUAL;

    std::cout << "=== VM8 frontpanel ===\n";
    std::cout << "o = power on,  p = power off\n";
    std::cout << "a = auto (run), m = manual\n";
    std::cout << "s = step (EN HEL klokkeperiode i MANUAL)\n";
    std::cout << "x = reset,      q = quit\n\n";

    bool quit = false;

    while (!quit) {
        bool clk = m.clock().isHigh();
        bool hlt = m.isHalted();

        std::cout << "[PWR=" << (power.isOn() ? "ON " : "OFF")
                  << " MODE=" << (mode == RunMode::AUTO ? "AUTO  " : "MANUAL")
                  << " CLK=" << (clk ? '1' : '0')
                  << " HLT=" << (hlt ? '1' : '0')
                  << "] > ";

        char cmd;
        if (!(std::cin >> cmd)) {
            break; // EOF / feil på stdin
        }

        switch (cmd) {
        case 'q':
        case 'Q':
            quit = true;
            break;

        case 'o':
        case 'O':
            power.turnOn();
            std::cout << "Strøm PÅ.\n";
            break;

        case 'p':
        case 'P':
            power.turnOff();
            mode = RunMode::MANUAL;
            std::cout << "Strøm AV.\n";
            break;

        case 'a':
        case 'A':
            if (power.isOn()) {
                mode = RunMode::AUTO;
                std::cout << "AUTO-modus.\n";
            } else {
                std::cout << "(kan ikke AUTO uten strøm)\n";
            }
            break;

        case 'm':
        case 'M':
            mode = RunMode::MANUAL;
            std::cout << "MANUAL-modus.\n";
            break;

        case 'x':
        case 'X':
            if (power.isOn()) {
                m.reset();
                std::cout << "Reset utført.\n";
            } else {
                std::cout << "(reset gir mest mening når strøm er på)\n";
            }
            break;

        case 's':
        case 'S':
            if (power.isOn() && mode == RunMode::MANUAL && !m.isHalted()) {
                // EN HEL klokkeperiode: RISING + FALLING
                stepper.singlePulse();
            } else {
                std::cout << "(kan ikke steppe nå – sjekk power/mode/HLT)\n";
            }
            break;

        default:
            std::cout << "(ukjent kommando)\n";
            break;
        }

        // AUTO-modus: én tick per loop-iterasjon (kan gjøres mer "live" senere)
        if (power.isOn() && mode == RunMode::AUTO && !m.isHalted()) {
            m.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cout << "\n=== VM8 avsluttet ===\n";
    return 0;
}
