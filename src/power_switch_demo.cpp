#include "power_switch.hpp"
#include <iostream>

int main() {
    PowerSwitch power;
    char choice;

    std::cout << "=== INTERAKTIV STRØMBRYTER ===\n";
    std::cout << "Tast:\n";
    std::cout << "  o  → slå PÅ strømmen\n";
    std::cout << "  f  → slå AV strømmen (f for 'off')\n";
    std::cout << "  q  → avslutt\n\n";

    do {
        std::cout << "\n🔌 Strømstatus: " << (power.isOn() ? "PÅ" : "AV") << "\n";
        std::cout << "Kommando [o/f/q]: ";
        std::cin >> choice;

        switch (choice) {
            case 'o':
                power.turnOn();
                break;
            case 'f':
                power.turnOff();
                break;
            case 'q':
                std::cout << "Avslutter demo...\n";
                break;
            default:
                std::cout << "Ugyldig valg.\n";
                break;
        }

    } while (choice != 'q');

    return 0;
}
