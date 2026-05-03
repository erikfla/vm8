// src/components/debugger.hpp
#pragma once
#include "../core/component.hpp"
#include "../core/signal_bus.hpp"
#include "../core/control.hpp"
#include "bus_register.hpp"
#include "prog_counter.hpp"
#include "step_counter.hpp"
#include <array>
#include <cstdint>
#include <string>

// Debugger – logikkanalysator med ring buffer-minne.
//
// Kobles til controlClk, etter alle input-porter men FØR step-teller.
// onRisingEdge() tar snapshot av alle registre i dette øyeblikket.

constexpr size_t DBG_DEPTH = 256;

class Debugger : public Component {
public:
    struct Snapshot {
        ControlWord ctrl       = 0;
        uint8_t     bus        = 0;
        uint8_t     a          = 0;
        uint8_t     b          = 0;
        uint8_t     pc         = 0;
        uint8_t     mar        = 0;
        uint8_t     ir         = 0;
        uint8_t     out        = 0;
        uint8_t     step       = 0;
        uint32_t    instrCount = 0;
        bool        halted     = false;
    };

    Debugger(SignalBus&   bus,
             BusRegister& regA,
             BusRegister& regB,
             BusRegister& regMAR,
             BusRegister& regIR,
             BusRegister& regOUT,
             ProgCounter& regPC,
             StepCounter& stepCtr,
             uint32_t&    instrCount)
        : bus_(bus), regA_(regA), regB_(regB), regMAR_(regMAR),
          regIR_(regIR), regOUT_(regOUT), regPC_(regPC),
          stepCtr_(stepCtr), instrCount_(instrCount) {}

    void set(const std::string&, bool) override {}
    bool get(const std::string&) const override { return false; }

    void onRisingEdge() override {
        if (frozen_) return;
        Snapshot s;
        s.ctrl       = bus_.ctrl();
        s.bus        = bus_.getData();
        s.a          = regA_.value();
        s.b          = regB_.value();
        s.pc         = regPC_.value();
        s.mar        = regMAR_.value();
        s.ir         = regIR_.value();
        s.out        = regOUT_.value();
        s.step       = stepCtr_.value();
        s.instrCount = instrCount_;
        s.halted     = bool(bus_.ctrl() & HLT);
        capture_[writePtr_] = s;
        writePtr_ = (writePtr_ + 1) % DBG_DEPTH;
        readPtr_  = writePtr_;
        if (count_ < DBG_DEPTH) count_++;
    }

    void freeze() { frozen_ = true; }
    void resume() { frozen_ = false; readPtr_ = writePtr_; }

    bool stepBack() {
        if (count_ < 1) return false;
        freeze();
        size_t oldest = (writePtr_ + DBG_DEPTH - count_) % DBG_DEPTH;
        if (readPtr_ == oldest) return false;
        readPtr_ = (readPtr_ + DBG_DEPTH - 1) % DBG_DEPTH;
        restore(capture_[readPtr_]);
        return true;
    }

    bool stepForward() {
        if (!frozen_) return false;
        size_t next = (readPtr_ + 1) % DBG_DEPTH;
        if (next == writePtr_) return false;
        readPtr_ = next;
        restore(capture_[readPtr_]);
        return true;
    }

    void setCheckpoint(const std::string& name) {
        checkpoint_ = readPtr_; checkpointName_ = name; hasCheckpoint_ = true;
    }
    bool jumpToCheckpoint() {
        if (!hasCheckpoint_) return false;
        freeze(); readPtr_ = checkpoint_; restore(capture_[readPtr_]); return true;
    }

    bool        frozen()         const { return frozen_; }
    size_t      count()          const { return count_; }
    std::string checkpointName() const { return checkpointName_; }
    bool        hasCheckpoint()  const { return hasCheckpoint_; }
    Snapshot    current()        const { return capture_[readPtr_]; }

    void reset() {
        writePtr_ = readPtr_ = count_ = 0;
        frozen_ = hasCheckpoint_ = false;
        checkpointName_ = "";
    }

private:
    void restore(const Snapshot& s) {
        bus_.setCtrl(s.ctrl);
        bus_.setData(s.bus);
        regA_.setValue(s.a);   regB_.setValue(s.b);
        regPC_.setValue(s.pc); regMAR_.setValue(s.mar);
        regIR_.setValue(s.ir); regOUT_.setValue(s.out);
        stepCtr_.setValue(s.step);
        instrCount_ = s.instrCount;
    }

    SignalBus&   bus_;
    BusRegister& regA_, &regB_, &regMAR_, &regIR_, &regOUT_;
    ProgCounter& regPC_;
    StepCounter& stepCtr_;
    uint32_t&    instrCount_;

    std::array<Snapshot, DBG_DEPTH> capture_ = {};
    size_t writePtr_ = 0, readPtr_ = 0, count_ = 0;
    bool   frozen_ = false, hasCheckpoint_ = false;
    size_t      checkpoint_     = 0;
    std::string checkpointName_ = "";
};
