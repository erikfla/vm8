// src/core/machine.hpp
#pragma once
#include "clock.hpp"
#include "signal_bus.hpp"
#include "control.hpp"
#include "microcode.hpp"
#include "../components/register8.hpp"
#include "../components/ram.hpp"
#include <array>
#include <cstdint>

// Machine er nettlisten – den kobler alle komponenter sammen
// og styrer hvem som får snakke på bussen når.
//
// Komponentene:
//   - Clock        – driver alt
//   - RAM          – 16 x 8-bit minne
//   - Register A   – akkumulator
//   - Register B   – operand til ALU
//   - PC           – program counter (4-bit)
//   - IR           – instruction register (8-bit)
//   - OUT          – output register

class Machine {
public:
    Machine();

    void reset();
    void tick();

    bool isHalted() const { return halted; }

    // Tilgang for WebSocket/JSON
    const Clock&     clock()   const { return clk; }
    const SignalBus& signals() const { return bus; }
    uint8_t          regA()    const { return a; }
    uint8_t          regB()    const { return b; }
    uint8_t          regMAR()  const { return mar; }
    uint8_t          regPC()   const { return pc; }
    uint8_t          regIR()   const { return ir; }
    uint8_t          regOUT()  const { return out; }
    uint8_t          ramAt(uint8_t addr) const { return ram[addr]; }

    ControlWord activeControl() const { return controlWord; }
    bool        flagC()         const { return flagCarry; }
    bool        flagZ()         const { return flagZero; }
    uint8_t     regStep()       const { return step; }

    void loadProgram(const std::array<uint8_t, 16>& program);

private:
    Clock     clk;
    SignalBus bus;

    // Registere
    uint8_t a   = 0;
    uint8_t b   = 0;
    uint8_t mar = 0;
    uint8_t pc  = 0;
    uint8_t ir  = 0;
    uint8_t out = 0;

    // RAM
    std::array<uint8_t, 16> ram = {};

    bool halted = false;

    uint8_t     step        = 0;
    ControlWord controlWord = NONE;
    bool        flagCarry   = false;
    bool        flagZero    = false;

    void handleEdge(Edge edge);
    void fetchAndExecute();
    void executeStep();
};
