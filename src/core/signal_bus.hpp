// src/core/signal_bus.hpp
#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

// SignalBus holder tilstanden til alle signaler i maskinen.
//
// Tenk på den som den fysiske bussen hos Ben Eater –
// én felles ledning alle komponenter kan lese fra og skrive til,
// men bare én om gangen (styrt av kontrollsignalene).
//
// I tillegg holder den 8-bit databussen som en uint8_t.

class SignalBus {
public:
    // Kontrollsignaler (én bit hver)
    void   set(const std::string& name, bool value);
    bool   get(const std::string& name) const;

    // 8-bit databus
    void    setData(uint8_t value);
    uint8_t getData() const;

    // Alle signaler – for JSON/WebSocket
    const std::unordered_map<std::string, bool>& all() const;

private:
    std::unordered_map<std::string, bool> signals;
    uint8_t data = 0;
};
