// src/core/signal_bus.hpp
#pragma once
#include <string>
#include <unordered_map>

class SignalBus {
public:
    // Sett/oppdater et signal (f.eks. "CLK", "HLT", "LOAD_A")
    void set(const std::string& name, bool value) {
        signals[name] = value;
    }

    // Les et signal. Hvis det ikke finnes, returner false som default.
    bool get(const std::string& name) const {
        auto it = signals.find(name);
        if (it == signals.end()) {
            return false;
        }
        return it->second;
    }

    // Gi tilgang til alle signaler (for logging/GUI senere)
    const std::unordered_map<std::string, bool>& all() const {
        return signals;
    }

private:
    std::unordered_map<std::string, bool> signals;
};
