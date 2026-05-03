// src/main.cpp
#include "core/machine.hpp"
#include "core/control.hpp"
#include "httplib.h"

#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
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
      << "}," 
      << "\"flags\":{" << "\"CF\":" << (machine.flagC() ? "true" : "false") << "," << "\"ZF\":" << (machine.flagZ() ? "true" : "false")
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
static void printHelp() {
    std::cout <<
"vm8 – 8-bit SAP-1 computer simulator\n"
"\n"
"USAGE\n"
"  ./vm8 [options]\n"
"\n"
"OPTIONS\n"
"  --run            Start in run mode (default: paused)\n"
"  --load <file>    Load binary program file (16 bytes raw)\n"
"  --debug          Enable ring-buffer debugger (256 snapshots)\n"
"  --verbose        Print JSON state to stdout on every /state poll\n"
"  --help           Show this help message\n"
"\n"
"ARCHITECTURE\n"
"  SAP-1 (Simple As Possible) – Ben Eater inspired\n"
"  8-bit data bus · 4-bit address space (16 bytes RAM)\n"
"  Two-phase clock: mainClk (outputs) / controlClk (inputs)\n"
"  5 microsteps per instruction (T0-T4)\n"
"\n"
"INSTRUCTION SET\n"
"  0x0  NOP            No operation\n"
"  0x1  LDA <addr>     Load RAM[addr] into A\n"
"  0x2  ADD <addr>     A = A + RAM[addr], update CF/ZF\n"
"  0x3  SUB <addr>     A = A - RAM[addr], update CF/ZF\n"
"  0x4  OUT            Output A to display\n"
"  0x6  JMP <addr>     Jump to addr\n"
"  0x7  JC  <addr>     Jump to addr if carry flag set\n"
"  0x8  JZ  <addr>     Jump to addr if zero flag set\n"
"  0xF  HLT            Halt\n"
"\n"
"FRONTEND\n"
"  Web UI at http://localhost:8765\n"
"  Includes assembler, RAM viewer, register display,\n"
"  control signals, microcode step, and flag LEDs.\n"
"\n"
"ASSEMBLER (in the web UI)\n"
"  LDA 14      ; load from address 14\n"
"  ADD 15      ; add value at address 15\n"
"  JC loop     ; jump to label if carry\n"
"  14: 200     ; data at address 14\n"
"  loop: OUT   ; label definition\n"
"\n"
"EXAMPLES\n"
"  ./vm8                        Start paused, open http://localhost:8765\n"
"  ./vm8 --run                  Start running immediately\n"
"  ./vm8 --load code.bin        Load program from file\n"
"  ./vm8 --load code.bin --run  Load and run\n"
"\n";
}

int main(int argc, char* argv[]) {
    std::string loadFile;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help")    { printHelp(); return 0; }
        if (arg == "--verbose") verbose   = true;
        if (arg == "--debug")   debugMode = true;
        if (arg == "--run")     mode      = Mode::RUN;
        if (arg == "--load" && i+1 < argc) loadFile = argv[++i];
    }
    if (!loadFile.empty()) {
        std::ifstream f(loadFile, std::ios::binary);
        if (!f) { std::cerr << "Error: cannot open " << loadFile << "\n"; return 1; }
        std::array<uint8_t, 16> prog{};
        f.read(reinterpret_cast<char*>(prog.data()), 16);
        machine.loadProgram(prog);
    }
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
    // POST /load – laster inn program fra frontend (16 bytes, kommaseparert)
    svr.Post("/load", [](const Request& req, Response& res) {
        std::array<uint8_t, 16> prog{};
        std::istringstream ss(req.body);
        std::string tok;
        int i = 0;
        while (std::getline(ss, tok, ',') && i < 16)
            prog[i++] = (uint8_t)std::stoi(tok);
        machine.loadProgram(prog);
        machine.reset();
        res.set_content("ok", "text/plain");
    });

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
