// src/core/control.hpp
#pragma once
#include <cstdint>

// Kontrollord – én bit per signal.
// Kombineres med bitwise OR: CO|MI betyr "PC ut og MAR inn samtidig"
//
// Navngiving følger Ben Eater sin SAP-1:
// Suffiks O = Output (ut på bussen)
// Suffiks I = Input  (inn fra bussen)

using ControlWord = uint32_t;

// ── Kontrollsignaler ─────────────────────────────────────
constexpr ControlWord NONE = 0;

constexpr ControlWord HLT  = 1 << 0;   // Halt
constexpr ControlWord MI   = 1 << 1;   // Memory Address Register In
constexpr ControlWord RI   = 1 << 2;   // RAM In
constexpr ControlWord RO   = 1 << 3;   // RAM Out
constexpr ControlWord IO   = 1 << 4;   // Instruction Register Out (operand)
constexpr ControlWord II   = 1 << 5;   // Instruction Register In
constexpr ControlWord AI   = 1 << 6;   // A Register In
constexpr ControlWord AO   = 1 << 7;   // A Register Out
constexpr ControlWord EO   = 1 << 8;   // ALU Out (sum)
constexpr ControlWord SU   = 1 << 9;   // Subtract
constexpr ControlWord BI   = 1 << 10;  // B Register In
constexpr ControlWord OI   = 1 << 11;  // Output Register In
constexpr ControlWord CE   = 1 << 12;  // Program Counter Enable (increment)
constexpr ControlWord CO   = 1 << 13;  // Program Counter Out
constexpr ControlWord J    = 1 << 14;  // Jump
constexpr ControlWord FI   = 1 << 15;  // Flags In
