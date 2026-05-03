// src/main.cpp
#include "core/machine.hpp"
#include "core/control.hpp"
#include "httplib.h"

#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <array>

using namespace httplib;

// ── Tilstand ─────────────────────────────────────────────
Machine machine;

enum class Mode { RUN, PAUSE };
Mode mode   = Mode::PAUSE;
float hz    = 1.0f;
std::atomic<bool> doStep { false };
bool oscHigh = false;
bool wasHalted  = false;  // 555-oscillator fase
bool verbose   = false;
bool debugMode = false;

// ── JSON ─────────────────────────────────────────────────
std::string toJSON() {
    std::ostringstream o;
    o << "{"
      << "\"clock\":\""  << (machine.clockHigh() ? "HIGH" : "LOW") << "\","
      << "\"osc\":\""   << (oscHigh ? "HIGH" : "LOW") << "\"," << "\n"
      << "\"bus\":"      << (int)machine.busData() << ","
      << "\"halted\":"   << (machine.isHalted() ? "true" : "false") << ","
      << "\"mode\":\""   << (mode == Mode::RUN ? "run" : "stopped") << "\","
      << "\"hz\":"       << hz << ","
      << "\"step\":"      << (int)machine.regStep()    << ","
      << "\"instr\":"     << (int)machine.instrCount() << ","
      << "\"dbg_enabled\":" << (debugMode ? "true" : "false") << ","
      << "\"dbg_frozen\":"  << (machine.dbgFrozen() ? "true" : "false") << ","
      << "\"dbg_depth\":"   << (int)machine.dbgCount()  << ","
      << "\"registers\":{"
        << "\"PC\":"  << (int)machine.regPC()  << ","
        << "\"MAR\":" << (int)machine.regMAR() << ","
        << "\"IR\":"  << (int)machine.regIR()  << ","
        << "\"A\":"   << (int)machine.regA()   << ","
        << "\"B\":"   << (int)machine.regB()   << ","
        << "\"ALU\":" << (int)machine.aluResult() << ","
        << "\"OUT\":" << (int)machine.regOUT()
      << "},"
      << "\"ram\":[";
    for (int i = 0; i < 16; i++) {
        o << (int)machine.ramAt(i);
        if (i < 15) o << ",";
    }
    o << "]";
    o << ",\"control\":{"
      << "\"HLT\":" << ((machine.activeControl() & HLT) ? "true" : "false") << ","
      << "\"MI\":"  << ((machine.activeControl() & MI)  ? "true" : "false") << ","
      << "\"RI\":"  << ((machine.activeControl() & RI)  ? "true" : "false") << ","
      << "\"RO\":"  << ((machine.activeControl() & RO)  ? "true" : "false") << ","
      << "\"IO\":"  << ((machine.activeControl() & IO)  ? "true" : "false") << ","
      << "\"II\":"  << ((machine.activeControl() & II)  ? "true" : "false") << ","
      << "\"AI\":"  << ((machine.activeControl() & AI)  ? "true" : "false") << ","
      << "\"AO\":"  << ((machine.activeControl() & AO)  ? "true" : "false") << ","
      << "\"EO\":"  << ((machine.activeControl() & EO)  ? "true" : "false") << ","
      << "\"SU\":"  << ((machine.activeControl() & SU)  ? "true" : "false") << ","
      << "\"BI\":"  << ((machine.activeControl() & BI)  ? "true" : "false") << ","
      << "\"OI\":"  << ((machine.activeControl() & OI)  ? "true" : "false") << ","
      << "\"CE\":"  << ((machine.activeControl() & CE)  ? "true" : "false") << ","
      << "\"CO\":"  << ((machine.activeControl() & CO)  ? "true" : "false") << ","
      << "\"J\":"   << ((machine.activeControl() & J)   ? "true" : "false") << ","
      << "\"FI\":"  << ((machine.activeControl() & FI)  ? "true" : "false")
      << ","
      << "\"flags\":{"
        << "\"CF\":" << (machine.flagC() ? "true" : "false") << ","
        << "\"ZF\":" << (machine.flagZ() ? "true" : "false")
      << "}}";
    return o.str();
}

// ── Kommandoer fra frontend ───────────────────────────────
void handleCommand(const std::string& msg) {
    if (msg == "step") {
        if (machine.dbgFrozen()) {
            mode = Mode::PAUSE;
            if (!machine.dbgStepForward()) {
                machine.dbgResume();
                doStep = true;
            }
        } else {
            doStep = true;
        }
        return;
    }
    if (msg == "run")        { mode = Mode::RUN;   machine.dbgResume(); return; }
    if (msg == "pause")      { mode = Mode::PAUSE; return; }
    if (msg == "reset")      { machine.reset(); wasHalted = false; return; }
    if (msg == "dbg_back")   { mode = Mode::PAUSE; machine.dbgStepBack();    return; }
    if (msg == "dbg_fwd")    { mode = Mode::PAUSE; if (!machine.dbgStepForward()) machine.dbgResume(); return; }
    if (msg == "dbg_chk")    { machine.dbgSetCheckpoint("A"); return; }
    if (msg == "dbg_jump")   { mode = Mode::PAUSE; machine.dbgJumpToCheckpoint(); return; }

    auto pos = msg.find("hz=");
    if (pos != std::string::npos) {
        float newHz = std::stof(msg.substr(pos + 3));
        if (newHz >= 0.05f && newHz <= 1000.0f) hz = newHz;
    }
}

// ── Main ─────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose") verbose   = true;
        if (arg == "--debug")   debugMode = true;
        if (arg == "--run")     mode      = Mode::RUN;
    }
    // Last inn testprogram
    std::array<uint8_t, 16> program = {
        0x1E,  // LDA 14
        0x2F,  // ADD 15
        0x40,  // OUT
        0xF0,  // HLT
        0,0,0,0,0,0,0,0,0,0,
        28, 14
    };
    machine.loadProgram(program);
    machine.setDebugMode(debugMode);
    if (debugMode) std::cout << "[Debugger] Aktivert (256-snapshot ring buffer)\n";

    Server svr;

    // Serve frontend
    svr.set_mount_point("/", "./frontend");

    // Tilstand
    svr.Get("/state", [](const Request&, Response& res) {
        std::string json = toJSON();
        if (verbose) std::cout << json << "\n";
        res.set_content(json, "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    // Kommandoer
    svr.Post("/cmd", [](const Request& req, Response& res) {
        handleCommand(req.body);
        res.set_content("ok", "text/plain");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    std::cout << "VM8 kjører på http://localhost:8765\n";

    // ── Event-løkke i bakgrunnen ──────────────────────────
    svr.set_pre_routing_handler([](const Request&, Response&) {
        return Server::HandlerResponse::Unhandled;
    });

    // Start server i egen tråd
    std::thread serverThread([&svr]() {
        svr.listen("0.0.0.0", 8765);
    });

    // ── Maskin-løkke i main ───────────────────────────────
    auto lastTick = std::chrono::steady_clock::now();
    auto lastOsc  = std::chrono::steady_clock::now();  // oscillator-timer

    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>
                   (now - lastTick).count();

        bool tick = false;

        // Oscillator (555-timer): toggler alltid på hz-takten, unntatt HLT
        {
            auto oscMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastOsc).count();
            int  halfPeriod = std::max(1, (int)(500.0f / hz));
            if (!machine.isHalted() && oscMs >= halfPeriod) {
                oscHigh = !oscHigh;
                lastOsc = now;
            }
        }
        if (mode == Mode::RUN && !machine.isHalted()) {
            if ((float)ms >= 1000.0f / std::max(0.05f, hz)) {
                int halfMs = (int)(500.0f / std::max(0.05f, hz));
                machine.tick();  // rising
                if (halfMs > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(halfMs));
                machine.tick();  // falling
                lastTick = now;
            }
        } else if (mode == Mode::PAUSE && doStep.exchange(false)) {
            int pulseMs = std::max(100, (int)(1000.0f / (hz * 2.0f)));
            oscHigh = true;
            machine.tick();  // HIGH
            std::this_thread::sleep_for(std::chrono::milliseconds(pulseMs));
            oscHigh = false;
            machine.tick();  // LOW
            std::this_thread::sleep_for(std::chrono::milliseconds(pulseMs));
        }

        if (tick) machine.tick();
        // Detekter HLT-overgang
        if (machine.isHalted() && !wasHalted) {
            std::cout << "[Machine] Halted\n";
            wasHalted = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    serverThread.join();
    return 0;
}
