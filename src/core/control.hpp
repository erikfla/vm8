// src/core/control.hpp
#pragma once
#include <cstdint>

// Kontrollord – 16-bit bitmask, ett bit per kontrollsignal.
// Tilsvarer Ben Eaters 16-bits kontrollinjen.

using ControlWord = uint16_t;

constexpr ControlWord NONE = 0;
constexpr ControlWord HLT  = 1 << 15;
constexpr ControlWord MI   = 1 << 14;
constexpr ControlWord RI   = 1 << 13;
constexpr ControlWord RO   = 1 << 12;
constexpr ControlWord IO   = 1 << 11;
constexpr ControlWord II   = 1 << 10;
constexpr ControlWord AI   = 1 << 9;
constexpr ControlWord AO   = 1 << 8;
constexpr ControlWord EO   = 1 << 7;
constexpr ControlWord SU   = 1 << 6;
constexpr ControlWord BI   = 1 << 5;
constexpr ControlWord OI   = 1 << 4;
constexpr ControlWord CE   = 1 << 3;
constexpr ControlWord CO   = 1 << 2;
constexpr ControlWord J    = 1 << 1;
constexpr ControlWord FI   = 1 << 0;
