// src/core/microcode.hpp
#pragma once
#include "control.hpp"
#include <array>

// Mikrokode-tabell
// Dimensjoner: [instruksjon][mikrosteg]
//
// Fetch-syklusen er lik for alle instruksjoner (steg 0-1):
//   Steg 0: CO|MI  – PC ut på buss, inn i MAR
//   Steg 1: RO|II|CE – RAM ut på buss, inn i IR, PC++
//
// Deretter kommer instruksjonsspesifikke steg (2-4)

constexpr int MAX_STEPS = 6;
constexpr int MAX_OPS   = 16;

constexpr std::array<std::array<ControlWord, MAX_STEPS>, MAX_OPS> MICROCODE = {{
    // 0x0 – NOP
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },

    // 0x1 – LDA
    { CO|MI, RO|II|CE, IO|MI, RO|AI, NONE, NONE },

    // 0x2 – ADD
    { CO|MI, RO|II|CE, IO|MI, RO|BI, EO|AI|FI, NONE },

    // 0x3 – SUB
    { CO|MI, RO|II|CE, IO|MI, RO|BI, EO|AI|SU|FI, NONE },

    // 0x4 – OUT
    { CO|MI, RO|II|CE, AO|OI, NONE, NONE, NONE },

    // 0x5-0xE – ikke implementert ennå
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },
    { CO|MI, RO|II|CE, NONE, NONE, NONE, NONE },

    // 0xF – HLT
    { CO|MI, RO|II|CE, HLT, NONE, NONE, NONE },
}};
