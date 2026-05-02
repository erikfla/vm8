// src/main.cpp
#include "core/machine.hpp"
#include "control.hpp"
#include "httplib.h"

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
int  hz     = 1;
std::atomic<bool> doStep { false };
bool verbose = false;

// ── JSON ─────────────────────────────────────────────────
std::string toJSON() {
    std::ostringstream o;
    o << "{"
      << "\"clock\":\""  << (machine.clock().isHigh() ? "HIGH" : "LOW") << "\","
      << "\"halted\":"   << (machine.isHalted() ? "true" : "false") << ","
      << "\"mode\":\""   << (mode == Mode::RUN ? "run" : "pause") << "\","
      << "\"hz\":"       << hz << ","
      << "\"registers\":{"
        << "\"PC\":"  << (int)machine.regPC()  << ","
        << "\"MAR\":" << (int)machine.regMAR() << ","
        << "\"IR\":"  << (int)machine.regIR()  << ","
        << "\"A\":"   << (int)machine.regA()   << ","
        << "\"B\":"   << (int)machine.regB()   << ","
        << "\"ALU\":" << (int)machine.regA()   << ","
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
      << "}}";
    return o.str();
}

// ── Kommandoer fra frontend ───────────────────────────────
void handleCommand(const std::string& msg) {
    if (msg == "step")  { doStep = true; return; }
    if (msg == "run")   { mode = Mode::RUN;   return; }
    if (msg == "pause") { mode = Mode::PAUSE; return; }
    if (msg == "reset") { machine.reset();    return; }

    auto pos = msg.find("hz=");
    if (pos != std::string::npos) {
        int newHz = std::stoi(msg.substr(pos + 3));
        if (newHz > 0 && newHz <= 1000) hz = newHz;
    }
}

// ── Main ─────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--verbose") verbose = true;
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

    // Start server i egen tråd (eneste tråd vi trenger)
    std::thread serverThread([&svr]() {
        svr.listen("0.0.0.0", 8765);
    });

    // ── Maskin-løkke i main ───────────────────────────────
    auto lastTick = std::chrono::steady_clock::now();

    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>
                   (now - lastTick).count();

        bool tick = false;

        if (mode == Mode::RUN && !machine.isHalted()) {
            if (ms >= 1000 / std::max(1, hz)) {
                machine.tick();
                lastTick = now;
            }
        } else if (mode == Mode::PAUSE && doStep.exchange(false)) {
            int pulseMs = std::max(100, 1000 / (hz * 2));
            machine.tick();  // HIGH
            std::this_thread::sleep_for(std::chrono::milliseconds(pulseMs));
            machine.tick();  // LOW
            std::this_thread::sleep_for(std::chrono::milliseconds(pulseMs));
        }

        if (tick) machine.tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    serverThread.join();
    return 0;
}
