#include "power_switch.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// ────── Terminalhåndtering ──────
void setNonBlocking(bool enable) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    if (enable)
        ttystate.c_lflag &= ~ICANON;
    else
        ttystate.c_lflag |= ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (enable)
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}
// ────────────────────────────────

int main() {
    PowerSwitch power;
    std::string timeline;
    const int maxTicks = 100;
    int tick = 0;
    char ch;

    std::cout << "=== INTERAKTIV STRØM-DEMO ===\n";
    std::cout << "Trykk 'o' = strøm PÅ, 'p' = strøm AV, 'q' = avslutt\n\n";
    setNonBlocking(true); // Aktiver non-blocking tastatur

    while (tick < maxTicks) {
        // Les tastetrykk om noen er trykket
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 'q') break;
            else if (ch == 'o') power.turnOn();
            else if (ch == 'p') power.turnOff();
        }

        timeline += power.isOn() ? "▮" : "_";
        std::cout << "\rStrøm: " << timeline << std::flush;

        ++tick;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    setNonBlocking(false); // Tilbakestill terminal
    std::cout << "\n=== DEMO FERDIG ===\n";
    return 0;
}
