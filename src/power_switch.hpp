#pragma once

class PowerSwitch {
private:
    bool on = false;

public:
    void turnOn() { on = true; }
    void turnOff() { on = false; }
    void toggle() { on = !on; }
    bool isOn() const { return on; }
};
