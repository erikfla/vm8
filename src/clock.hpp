#pragma once
#include <string>

enum class Mode { STEP, RUN, HALT };

class Clock {
private:
    bool state = false;
    bool lastState = false;

public:
    void tick();                      // én puls
    bool isRisingEdge() const;       // LOW → HIGH
    bool isFallingEdge() const;      // HIGH → LOW
    std::string visual() const;      // "▮" eller "_"
};
