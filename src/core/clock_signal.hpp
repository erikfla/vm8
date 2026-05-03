// src/core/clock_signal.hpp
#pragma once
#include "component.hpp"
#include <vector>

// ClockSignal – én klokkelinje komponenter kobles til.
//
// Analogi: en fysisk linje på breadboardet (CLK eller /CLK).
// Komponenter kobler seg til linjen med connect().
// Når linjen stiger, kaller den onRisingEdge() på alle lyttere –
// i tilkoblingsrekkefølge. Rekkefølgen er nettlistens ansvar
// (Machine-konstruktøren), akkurat som ledningsrekkefølge på brettet.
//
// Fall propageres ikke – alle komponenter er rising-edge-trigget.

class ClockSignal {
public:
    void connect(Component* c) { listeners_.push_back(c); }

    void rise() {
        high_ = true;
        for (auto* c : listeners_) c->onRisingEdge();
    }

    void fall() { high_ = false; }

    bool isHigh() const { return high_; }

private:
    bool high_ = false;
    std::vector<Component*> listeners_;
};
