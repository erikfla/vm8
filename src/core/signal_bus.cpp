// src/core/signal_bus.cpp
#include "signal_bus.hpp"

void SignalBus::set(const std::string& name, bool value) {
    signals[name] = value;
}

bool SignalBus::get(const std::string& name) const {
    auto it = signals.find(name);
    if (it == signals.end()) return false;
    return it->second;
}

void SignalBus::setData(uint8_t value) {
    data = value;
}

uint8_t SignalBus::getData() const {
    return data;
}

const std::unordered_map<std::string, bool>& SignalBus::all() const {
    return signals;
}
