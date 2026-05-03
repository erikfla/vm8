// src/core/component.hpp
#pragma once
#include <string>

// Component – abstrakt grensesnitt for alle kretskort-komponenter.
//
// Alle komponenter kobler seg til SignalBus og reagerer på
// rising/falling edge – akkurat som ekte TTL-brikker.

class Component {
public:
    virtual ~Component() = default;

    virtual void set(const std::string& signal, bool value) = 0;
    virtual bool get(const std::string& signal) const = 0;

    virtual void onRisingEdge()  {}  // default: ingen handling
    virtual void onFallingEdge() {}
};
