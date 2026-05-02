// src/core/machine.hpp
#pragma once
#include "clock.hpp"
#include "signal_bus.hpp"
#include "control.hpp"
#include "microcode.hpp"
#include "../components/bus_register.hpp"
#include "../components/prog_counter.hpp"
#include "../components/bus_alu.hpp"
#include "../components/bus_ram.hpp"
#include "../components/step_counter.hpp"
#include <array>
#include <cstdint>

// Machine er nettlisten – den kobler alle komponenter
// og driver dem med klokkesignalet. Ingen logikk her,
// bare ledninger.
//
// Topology (som Ben Eaters breadboard):
//
//   Clock → rising edge  → alle utganger driver bussen
//         → falling edge → alle innganger latcher + step++
//
//   Komponenter:
//     regPC   : ProgCounter  (CO=ut, CE=inkrement, J=hopp)
//     regMAR  : BusRegister  (MI=inn)
//     regIR   : BusRegister  (IO=ut operand, II=inn)
//     regA    : BusRegister  (AO=ut, AI=inn)
//     regB    : BusRegister  (BI=inn)
//     regOUT  : BusRegister  (OI=inn)
//     alu     : BusALU       (EO=ut, SU=subtract, FI=flagg inn)
//     ram     : BusRAM       (RO=ut, RI=inn) – adressert av regMAR

class Machine {
public:
    Machine();

    void reset();
    void tick();

    bool isHalted() const { return halted_; }

    // ── Tilgang for JSON ──────────────────────────────────
    const Clock&     clock()   const { return clk_; }
    const SignalBus& signals() const { return bus_; }

    uint8_t regA()   const { return regA_.value(); }
    uint8_t regB()   const { return regB_.value(); }
    uint8_t regMAR() const { return regMAR_.value(); }
    uint8_t regPC()  const { return regPC_.value(); }
    uint8_t regIR()  const { return regIR_.value(); }
    uint8_t regOUT() const { return regOUT_.value(); }
    uint8_t regStep()   const { return step_.value(); }
    uint32_t instrCount() const { return instrCount_; }

    uint8_t     ramAt(uint8_t addr) const { return ram_.at(addr); }
    ControlWord activeControl()     const { return bus_.ctrl(); }
    bool        flagC()             const { return alu_.flagCarry(); }
    bool        flagZ()             const { return alu_.flagZero(); }

    void loadProgram(const std::array<uint8_t, 16>& program);

private:
    Clock     clk_;
    SignalBus bus_;

    // ── Komponenter i deklarasjonsrekkefølge ─────────────
    // (rekkefølge betyr noe: bus_ må eksistere først)
    BusRegister  regMAR_  { bus_, NONE, MI         };
    BusRegister  regA_    { bus_, AO,   AI         };
    BusRegister  regB_    { bus_, NONE, BI         };
    BusRegister  regIR_   { bus_, IO,   II,  0x0F  };  // IO sender bare nibble
    BusRegister  regOUT_  { bus_, NONE, OI         };
    ProgCounter  regPC_   { bus_                };
    BusRAM       ram_     { bus_, regMAR_        };
    BusALU       alu_     { bus_, regA_, regB_   };
    StepCounter  step_;
    uint32_t     instrCount_ = 0;

    bool    halted_ = false;

    void risingEdge();
    void fallingEdge();
};
