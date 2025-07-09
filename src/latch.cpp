#include "latch.hpp"

bool nor(bool a, bool b) {
    return !(a || b);
}

void SRLatch::set(bool s, bool r) {
    q = nor(r, nq);
    nq = nor(s, q);
}

bool SRLatch::output() const {
    return q;
}

void SRLatch::reset() {
    q = false;
    nq = true;
}

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
