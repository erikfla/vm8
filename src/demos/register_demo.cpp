#include "register1bit.hpp"
#include <iostream>

int main() {
    Register1bit reg;

    // I Ben Eater-ånd: vi "trykker på reset" før vi bruker registeret
    reg.reset();

    std::cout << "1. Initial verdi (skal være 0): " << reg.value() << "\n";

    reg.load(true, true); // Sett 1 med enable = true
    std::cout << "2. Etter load(1, 1): " << reg.value() << "\n";

    reg.load(false, false); // Prøv å sette 0 med enable = false
    std::cout << "3. Etter load(0, 0) (skal fortsatt være 1): " << reg.value() << "\n";

    reg.load(false, true); // Nå faktisk skriv 0 inn
    std::cout << "4. Etter load(0, 1): " << reg.value() << "\n";

    return 0;
}
