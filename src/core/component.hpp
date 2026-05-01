// src/core/component.hpp
#pragma once
#include <string>

// Grensesnittet alle hardware-komponenter må implementere.
//
// En komponent modellerer én fysisk krets – en flipflop, et register,
// en ALU, eller en proxy mot ekte hardware. Maskinen kobler komponenter
// sammen via SignalBus og driver dem med Clock.
//
// Analogi: pinnene på en 74LS-krets. Du setter innganger, leser utganger,
// og komponenten reagerer på klokkekanten.

class Component {
public:
    // Sett en inngang (som å legge HIGH/LOW på en pinne)
    virtual void set(const std::string& pin, bool value) = 0;

    // Les en utgang
    virtual bool get(const std::string& pin) const = 0;

    // Klokken gikk LOW → HIGH
    virtual void onRisingEdge()  {}

    // Klokken gikk HIGH → LOW
    virtual void onFallingEdge() {}

    // Alltid virtual destructor i C++ når du bruker arv
    virtual ~Component() = default;
};
