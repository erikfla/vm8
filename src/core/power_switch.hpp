// src/core/power_switch.hpp
#pragma once

class PowerSwitch {
private:
    bool on_ = false;

public:
    void turnOn()  { on_ = true; }
    void turnOff() { on_ = false; }
    bool isOn() const { return on_; }
};
