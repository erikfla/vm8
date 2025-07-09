#include "clock.hpp"
#include "latch.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

int main() {
    Clock clk(1.0); // 1 Hz (1 tick per sekund)
    DLatch bit;

    bool input = false;
    bool enable = true;  // Om klokka er koblet til latch
    std::atomic<bool> running(false);
    std::string cmd;

    std::cout << "=== Klokke + Latch Demo ===\n\n";
    std::cout << "Kommandoer:\n";
    std::cout << "  0 / 1   → sett input-bit\n";
    std::cout << "  s       → ett klokkeslag\n";
    std::cout << "  r       → start kontinuerlig klokke\n";
    std::cout << "  p       → pause (stopp klokke)\n";
    std::cout << "  t       → toggle enable (koble klokka til/fra latch)\n";
    std::cout << "  q       → avslutt\n\n";

    // Tråd som kjører klokka og oppdaterer latch
    std::thread clock_thread([&]() {
        while (true) {
            if (running) {
                clk.tick();
                if (enable)
                    bit.update(input, clk.state());

                std::cout << "[clk: " << (clk.state() ? "↑" : "_")
                          << "  in: " << input
                          << "  out: " << bit.output()
                          << "]" << std::endl;

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });

    // Interaktiv kontroll
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, cmd);

        if (cmd == "q") break;
        else if (cmd == "0") input = false;
        else if (cmd == "1") input = true;
        else if (cmd == "t") enable = !enable;
        else if (cmd == "s") {
            clk.tick();
            if (enable)
                bit.update(input, clk.state());

            std::cout << "[clk: " << (clk.state() ? "↑" : "_")
                      << "  in: " << input
                      << "  out: " << bit.output()
                      << "]" << std::endl;
        }
        else if (cmd == "r") running = true;
        else if (cmd == "p") running = false;
        else std::cout << "Ukjent kommando\n";
    }

    std::cout << "\n=== Avslutter ===\n";
    running = false;
    clock_thread.detach(); // Vi gir slipp – demo ferdig

    return 0;
}
