// src/core/clock.hpp
#pragma once
#include "component.hpp"

enum class Edge { None, Rising, Falling };

// Clock – to-fase klokke som Ben Eaters 555-timer.
//
// tick() veksler tilstand og returnerer hvilken flanke som oppstod.
// isHigh() brukes til å sjekke gjeldende klokkefase.

class Clock : public Component {
public:
    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return high_; }

    Edge tick() {
        high_ = !high_;
        return high_ ? Edge::Rising : Edge::Falling;
    }

    bool isHigh() const { return high_; }
    void reset()        { high_ = false; }

private:
    bool high_ = false;
};
