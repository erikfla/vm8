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
// JC/JZ: tabellen inneholder ikke-tatt-varianten (NONE på T2).
// MicrocodeROM overrider T2 med IO|J hvis flagget er satt.

constexpr int MAX_STEPS = 5;
constexpr int MAX_OPS   = 16;

constexpr std::array<std::array<ControlWord, MAX_STEPS>, MAX_OPS> MICROCODE = {{
    // 0x0 – NOP
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },

    // 0x1 – LDA addr
    { CO|MI, RO|II|CE, IO|MI, RO|AI, NONE },

    // 0x2 – ADD addr
    { CO|MI, RO|II|CE, IO|MI, RO|BI, EO|AI|FI },

    // 0x3 – SUB addr
    { CO|MI, RO|II|CE, IO|MI, RO|BI, EO|AI|SU|FI },

    // 0x4 – OUT
    { CO|MI, RO|II|CE, AO|OI, NONE,  NONE },

    // 0x5 – STA addr  (store A to RAM[addr])
    { CO|MI, RO|II|CE, IO|MI, AO|RI, NONE },

    // 0x6 – JMP addr  (ubetinget hopp)
    { CO|MI, RO|II|CE, IO|J,  NONE,  NONE },

    // 0x7 – JC addr   (hopp hvis carry; MicrocodeROM overrider T2)
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },

    // 0x8 – JZ addr   (hopp hvis zero; MicrocodeROM overrider T2)
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },

    // 0x9 – LDI imm  (load 4-bit immediate into A)
    { CO|MI, RO|II|CE, IO|AI, NONE,  NONE },
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },
    { CO|MI, RO|II|CE, NONE,  NONE,  NONE },

    // 0xF – HLT
    { CO|MI, RO|II|CE, HLT,   NONE,  NONE },
}};
