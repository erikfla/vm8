// src/main.cpp
#include "core/machine.hpp"
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

enum class Mode { MANUAL, AUTO };
Mode mode   = Mode::MANUAL;
int  hz     = 1;
bool doStep = false;

// ── JSON ─────────────────────────────────────────────────
std::string toJSON() {
    std::ostringstream o;
    o << "{"
      << "\"clock\":\""  << (machine.clock().isHigh() ? "HIGH" : "LOW") << "\","
      << "\"halted\":"   << (machine.isHalted() ? "true" : "false") << ","
      << "\"mode\":\""   << (mode == Mode::AUTO ? "auto" : "manual") << "\","
      << "\"hz\":"       << hz << ","
      << "\"registers\":{"
        << "\"PC\":"  << (int)machine.regPC()  << ","
        << "\"IR\":"  << (int)machine.regIR()  << ","
        << "\"A\":"   << (int)machine.regA()   << ","
        << "\"OUT\":" << (int)machine.regOUT()
      << "},"
      << "\"ram\":[";
    for (int i = 0; i < 16; i++) {
        o << (int)machine.ramAt(i);
        if (i < 15) o << ",";
    }
    o << "]}";
    return o.str();
}

// ── Kommandoer fra frontend ───────────────────────────────
void handleCommand(const std::string& msg) {
    if (msg.find("step")   != std::string::npos) doStep = true;
    if (msg.find("manual") != std::string::npos) mode = Mode::MANUAL;
    if (msg.find("auto")   != std::string::npos) mode = Mode::AUTO;
    if (msg.find("reset")  != std::string::npos) machine.reset();

    auto pos = msg.find("hz=");
    if (pos != std::string::npos) {
        int newHz = std::stoi(msg.substr(pos + 3));
        if (newHz > 0 && newHz <= 1000) hz = newHz;
    }
}

// ── Main ─────────────────────────────────────────────────
int main() {
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
        res.set_content(toJSON(), "application/json");
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

        if (mode == Mode::AUTO && !machine.isHalted()) {
            if (ms >= 1000 / hz) {
                tick     = true;
                lastTick = now;
            }
        } else if (mode == Mode::MANUAL && doStep) {
            tick   = true;
            doStep = false;
        }

        if (tick) machine.tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    serverThread.join();
    return 0;
}
