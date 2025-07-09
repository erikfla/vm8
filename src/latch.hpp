#pragma once

class SRLatch {
private:
    bool q = false;
    bool nq = true;

public:
    void set(bool s, bool r);
    bool output() const;
    void reset();
};

class DLatch {
private:
    SRLatch latch;

public:
    void update(bool d, bool enable); // enable = "klokkesignal"
    bool output() const;
    void reset();
};
