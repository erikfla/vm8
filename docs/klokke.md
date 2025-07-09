# Klokke – Puls og Rytme i VM8

## Hva er klokka?

I en digital datamaskin er klokka et kontrollsignal som tikker jevnt og gir rytme til hele systemet. Den sørger for at komponenter som registre, latcher og ALU jobber i takt.

## Clock-klassen

Vår virtuelle klokke er implementert i `Clock`-klassen:

```cpp
class Clock {
public:
    Clock(double hz = 1.0);    // Frekvens i hertz
    void tick();               // Et steg (høy → lav eller lav → høy)
    bool state() const;        // Nåværende nivå (true = høy, false = lav)
};
