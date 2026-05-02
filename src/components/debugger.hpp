// src/components/debugger.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include "bus_register.hpp"
#include "prog_counter.hpp"
#include "step_counter.hpp"
#include "bus_ram.hpp"
#include <array>
#include <cstdint>
#include <string>

// Debugger – logikkanalysator med ring buffer-minne.
//
// Analogi: Agilent/Tektronix logikkanalysator koblet til bussen.
// Maskinen vet ikke at den finnes – debuggeren er ren observatør
// på rising edge, og aktiv skriver (JTAG-stil) ved BACK/FORWARD.
//
// Capture-RAM: 256 snapshots i ring buffer.
// Checkpoint:  named adresse i ring bufferet.
//
//  Write pointer → rykker frem for hvert rising edge
//  Read pointer  → følger write normalt, kan spoles bakover
//  Frozen        → write stopper, read navigerer fritt

constexpr size_t DBG_DEPTH = 256;

class Debugger : public Component {
public:
    struct Snapshot {
        ControlWord ctrl   = 0;
        uint8_t     bus    = 0;
        uint8_t     a      = 0;
        uint8_t     b      = 0;
        uint8_t     pc     = 0;
        uint8_t     mar    = 0;
        uint8_t     ir     = 0;
        uint8_t     out    = 0;
        uint8_t     step   = 0;
        bool        halted = false;
    };

    Debugger(SignalBus&   bus,
             BusRegister& regA,
             BusRegister& regB,
             BusRegister& regMAR,
             BusRegister& regIR,
             BusRegister& regOUT,
             ProgCounter& regPC,
             StepCounter& stepCtr,
             bool&        halted)
        : bus_(bus), regA_(regA), regB_(regB), regMAR_(regMAR),
          regIR_(regIR), regOUT_(regOUT), regPC_(regPC),
          stepCtr_(stepCtr), halted_(halted) {}

    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    // Rising edge: ta snapshot og legg i ring buffer
    void onRisingEdge() override {
        if (frozen_) return;

        Snapshot s;
        s.ctrl   = bus_.ctrl();
        s.bus    = bus_.getData();
        s.a      = regA_.value();
        s.b      = regB_.value();
        s.pc     = regPC_.value();
        s.mar    = regMAR_.value();
        s.ir     = regIR_.value();
        s.out    = regOUT_.value();
        s.step   = stepCtr_.value();
        s.halted = halted_;

        capture_[writePtr_] = s;
        writePtr_ = (writePtr_ + 1) % DBG_DEPTH;
        readPtr_  = writePtr_;  // følger write normalt
        if (count_ < DBG_DEPTH) count_++;
    }

    // Frys – stopp capture, aktiver navigering
    void freeze() { frozen_ = true; }
    void resume() { frozen_ = false; readPtr_ = writePtr_; }

    // Naviger bakover – restore forrige snapshot til alle registre
    bool stepBack() {
        if (count_ < 2) return false;
        freeze();
        readPtr_ = (readPtr_ + DBG_DEPTH - 1) % DBG_DEPTH;
        if (readPtr_ == writePtr_) return false;
        restore(capture_[readPtr_]);
        return true;
    }

    // Naviger fremover
    bool stepForward() {
        if (!frozen_) return false;
        size_t next = (readPtr_ + 1) % DBG_DEPTH;
        if (next == writePtr_) { resume(); return true; }
        readPtr_ = next;
        restore(capture_[readPtr_]);
        return true;
    }

    // Sett checkpoint ved gjeldende posisjon
    void setCheckpoint(const std::string& name) {
        checkpoint_      = readPtr_;
        checkpointName_  = name;
        hasCheckpoint_   = true;
    }

    // Hopp til checkpoint
    bool jumpToCheckpoint() {
        if (!hasCheckpoint_) return false;
        freeze();
        readPtr_ = checkpoint_;
        restore(capture_[readPtr_]);
        return true;
    }

    bool        frozen()          const { return frozen_; }
    size_t      count()           const { return count_; }
    std::string checkpointName()  const { return checkpointName_; }
    bool        hasCheckpoint()   const { return hasCheckpoint_; }
    Snapshot    current()         const { return capture_[readPtr_]; }

    void reset() {
        writePtr_ = readPtr_ = count_ = 0;
        frozen_ = hasCheckpoint_ = false;
        checkpointName_ = "";
    }

private:
    void restore(const Snapshot& s) {
        bus_.setCtrl(s.ctrl);
        bus_.setData(s.bus);
        regA_.setValue(s.a);
        regB_.setValue(s.b);
        regPC_.setValue(s.pc);
        regMAR_.setValue(s.mar);
        regIR_.setValue(s.ir);
        regOUT_.setValue(s.out);
        stepCtr_.clear();
        // step er read-only fra StepCounter – vi setter den ved å telle
        // (forenklet: godtar litt avvik på step ved restore)
        halted_ = s.halted;
    }

    SignalBus&   bus_;
    BusRegister& regA_;
    BusRegister& regB_;
    BusRegister& regMAR_;
    BusRegister& regIR_;
    BusRegister& regOUT_;
    ProgCounter& regPC_;
    StepCounter& stepCtr_;
    bool&        halted_;

    std::array<Snapshot, DBG_DEPTH> capture_ = {};
    size_t writePtr_      = 0;
    size_t readPtr_       = 0;
    size_t count_         = 0;
    bool   frozen_        = false;

    bool        hasCheckpoint_  = false;
    size_t      checkpoint_     = 0;
    std::string checkpointName_ = "";
};
