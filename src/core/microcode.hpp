// src/core/microcode.hpp
#pragma once
#include "control.hpp"
#include <array>

// Mikrokodetabellen – ROM med instruksjoner × mikrosteg.
//
// Ben Eater SAP-1 bruker 3 mikrosteg-bits → 8 steg (T0–T7).
// Vi bruker 6 steg (T0–T5) for å holde det enkelt, men
// ALLE instruksjoner kjører alltid alle steg – NONE = NOP-mikrosteg.
// Dette er hardware-akkurat: ingen logikk stopper telleren.

constexpr int MAX_STEPS = 6;

// Opkoder
constexpr uint8_t OP_NOP = 0x0;
constexpr uint8_t OP_LDA = 0x1;
constexpr uint8_t OP_ADD = 0x2;
constexpr uint8_t OP_SUB = 0x3;
constexpr uint8_t OP_OUT = 0x4;
constexpr uint8_t OP_HLT = 0xF;

// Mikrokodetabell [opcode][step]
inline constexpr std::array<std::array<ControlWord, MAX_STEPS>, 16> MICROCODE = {{
  // 0x0 NOP
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  // 0x1 LDA
  {{ CO|MI, RO|II|CE, IO|MI, RO|AI, NONE, NONE }},
  // 0x2 ADD
  {{ CO|MI, RO|II|CE, IO|MI, RO|BI, EO|AI|FI, NONE }},
  // 0x3 SUB
  {{ CO|MI, RO|II|CE, IO|MI, RO|BI, EO|AI|SU|FI, NONE }},
  // 0x4 OUT
  {{ CO|MI, RO|II|CE, AO|OI, NONE, NONE, NONE }},
  // 0x5–0xE: ubrukte → NOP
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  {{ CO|MI, RO|II|CE, NONE, NONE, NONE, NONE }},
  // 0xF HLT
  {{ CO|MI, RO|II|CE, HLT,  NONE, NONE, NONE }},
}};
