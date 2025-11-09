#include "latch.hpp"

// bool nor(bool a, bool b) { return !(a || b); }
// (Ikke brukt i denne implementasjonen, men nyttig for å vise
//  hvordan SR-latchen kan bygges fra grunnleggende NOR-porter.)

// Vi kunne brukt nor(...) inni også, men vi gjør det heller enklere og mer eksplisitt.

void SRLatch::set(bool s, bool r) {
    // Klassisk SR-latch:
    // s = 1, r = 0  → SET  (Q = 1, /Q = 0)
    // s = 0, r = 1  → RESET(Q = 0, /Q = 1)
    // s = 0, r = 0  → HOLD (behold forrige verdi)
    // s = 1, r = 1  → "ugyldig" – vi velger å bare holde

    if (s && !r) {
        // SET
        q  = true;
        nq = false;
    } else if (!s && r) {
        // RESET
        q  = false;
        nq = true;
    } else if (!s && !r) {
        // HOLD – gjør ingenting, behold q og nq
    } else {
        // s == 1 og r == 1 – vi velger å holde for enkelhets skyld
        // (kunne logget en advarsel i en mer avansert simulering)
    }
}

bool SRLatch::output() const {
    return q;
}

void SRLatch::reset() {
    // Definert starttilstand: Q = 0, /Q = 1
    q  = false;
    nq = true;
}

// D-latch bygget på SR-latch
void DLatch::update(bool d, bool enable) {
    bool s = d && enable;
    bool r = (!d) && enable;
    latch.set(s, r);
}

bool DLatch::output() const {
    return latch.output();
}

void DLatch::reset() {
    latch.reset();
}
