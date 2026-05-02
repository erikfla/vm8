// src/core/clock.hpp
#pragma once
#include "component.hpp"

// Klokken driver hele maskinen.
//
// Hver tick() veksler mellom HIGH og LOW og returnerer
// hvilken kant som nettopp skjedde.
//
// Pinner:
//   Ut:  CLK  (HIGH eller LOW)

enum class Edge {
    None,
    Rising,   // LOW → HIGH
    Falling   // HIGH → LOW
};

class Clock : public Component {
public:
    void set(const std::string& /*pin*/, bool /*value*/) override {}
    bool get(const std::string& pin) const override;

    // Én halvpuls – returnerer kanten som skjedde
    Edge tick();

    void reset();
    bool isHigh() const { return state; }

private:
    bool state = false;
};
