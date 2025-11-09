#pragma once

// En enkel SR-latch basert på NOR-logikk
class SRLatch {
private:
    bool q;
    bool nq;

public:
    // Sett S og R (typisk fra D-latchlogikk)
    void set(bool s, bool r);

    // Les ut Q
    bool output() const;

    // Sett definert starttilstand (Q = 0, /Q = 1)
    void reset();
};

// D-latch bygget på SR-latch
class DLatch {
private:
    SRLatch latch;

public:
    // d = data, enable = om vi skriver nå eller bare holder
    void update(bool d, bool enable);

    bool output() const;

    void reset();
};
