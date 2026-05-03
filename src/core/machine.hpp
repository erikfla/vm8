// src/core/machine.hpp
#pragma once
#include "signal_bus.hpp"
#include "main_clock.hpp"
#include "control.hpp"
#include "microcode.hpp"
#include "../components/bus_register.hpp"
#include "../components/prog_counter.hpp"
#include "../components/bus_alu.hpp"
#include "../components/bus_ram.hpp"
#include "../components/step_counter.hpp"
#include "../components/microcode_rom.hpp"
#include "../components/debugger.hpp"
#include <array>
#include <cstdint>
#include <iostream>

// Machine er nettlisten – konstruktøren kobler alle porter til klokkelinjene.
//
//   mainClk    → PC, RAM, A, IR, ALU (utganger driver bussen)
//   controlClk → alle input-porter, OutDisplay, Debugger,
//                StepCounter, InstrCounter, MicrocodeROM
//
// HLT gater MainClock direkte. isHalted() leser kontrollordet.
// Machine::tick() = clk_.tick() – én linje.

class Machine {
public:
    Machine();

    void reset();
    void tick() { clk_.tick(); }

    bool isHalted()  const { return bool(bus_.ctrl() & HLT); }
    bool clockHigh() const { return clk_.isHigh(); }

    uint8_t  regA()       const { return regA_.value(); }
    uint8_t  regB()       const { return regB_.value(); }
    uint8_t  regMAR()     const { return regMAR_.value(); }
    uint8_t  regPC()      const { return regPC_.value(); }
    uint8_t  regIR()      const { return regIR_.value(); }
    uint8_t  regOUT()     const { return regOUT_.value(); }
    uint8_t  regStep()    const { return step_.value(); }
    uint32_t instrCount() const { return instrCount_; }

    uint8_t     ramAt(uint8_t addr) const { return ram_.at(addr); }
    ControlWord activeControl()     const { return bus_.ctrl(); }
    uint8_t     aluResult()          const { return alu_.compute(); }
    bool        outActive()         const { return outDisplay_.active; }
    bool        flagC()             const { return alu_.flagCarry(); }
    bool        flagZ()             const { return alu_.flagZero(); }
    uint8_t     busData()           const { return bus_.getData(); }

    void setDebugMode(bool on) { debugMode_ = on; }
    bool debugMode()     const { return debugMode_; }
    bool dbgStepBack()        { return dbg_.stepBack(); }
    bool dbgStepForward()     { return dbg_.stepForward(); }
    void dbgSetCheckpoint(const std::string& name) { dbg_.setCheckpoint(name); }
    bool dbgJumpToCheckpoint(){ return dbg_.jumpToCheckpoint(); }
    void dbgResume()          { dbg_.resume(); }
    bool dbgFrozen()    const { return dbg_.frozen(); }
    size_t dbgCount()   const { return dbg_.count(); }

    void loadProgram(const std::array<uint8_t, 16>& program);

private:
    // bus_ MÅ deklareres før clk_ (initialiseringsrekkefølge)
    SignalBus  bus_;
    MainClock  clk_ { bus_ };

    BusRegister  regMAR_  { bus_, NONE, MI        };
    BusRegister  regA_    { bus_, AO,   AI        };
    BusRegister  regB_    { bus_, NONE, BI        };
    BusRegister  regIR_   { bus_, IO,   II,  0x0F };
    BusRegister  regOUT_  { bus_, NONE, OI        };
    ProgCounter  regPC_   { bus_                  };
    BusRAM       ram_     { bus_, regMAR_         };
    BusALU       alu_     { bus_, regA_, regB_    };
    StepCounter  step_;
    MicrocodeROM rom_     { bus_, regIR_, step_, alu_ };
    uint32_t     instrCount_ = 0;

    // Kombinatorisk display – som 7-segment koblet til OUT-register
    struct OutDisplay : Component {
        Machine& m;
        uint8_t  last   = 0;
        bool     active = false;
        explicit OutDisplay(Machine& machine) : m(machine) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (!(m.activeControl() & OI)) return;
            // Les buss-verdien direkte (stabil fra mainClk-fasen)
            uint8_t v = static_cast<uint8_t>(m.bus_.getData());
            active = true;
            if (v != last) {
                last = v;
                std::cout << "[OUT] " << std::dec << (int)v << "\n";
            }
        }
    } outDisplay_ { *this };

    // Instruksjonsteller – detekterer T5→T0 wrap
    struct InstrCounter : Component {
        Machine& m;
        explicit InstrCounter(Machine& machine) : m(machine) {}
        void set(const std::string&, bool) override {}
        bool get(const std::string&) const override { return false; }
        void onRisingEdge() override {
            if (m.step_.value() == 0) m.instrCount_++;
        }
    } instrCounter_ { *this };

    Debugger dbg_ { bus_, regA_, regB_, regMAR_, regIR_, regOUT_,
                    regPC_, step_, instrCount_ };

    bool debugMode_ = false;
};
