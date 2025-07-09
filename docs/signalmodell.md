# ⏱️ Signalmodell for VM8

Dette notatet forklarer hvordan vi modellerer klokkesignal og hvordan komponenter måler og tolker det i vårt virtuelle maskinprosjekt.

## 💡 Digital klokke med abstrakt grensesnitt

I stedet for å bruke rå boolske verdier direkte, benytter vi metoder som:

- `isHigh()` — returnerer true hvis signalet er høyt
- `isLow()` — returnerer true hvis signalet er lavt
- `isRisingEdge()` — detekterer overgang fra LOW → HIGH
- `isFallingEdge()` — detekterer overgang fra HIGH → LOW

Fordelene med dette er:
- Vi kan enkelt endre implementasjonen bak uten å endre klientkode
- Vi kan senere bytte fra bool → spenning → analog simulering
- Vi får et ryddig og intuitivt grensesnitt mot klokke og kontrollsignaler

## 🔌 Fremtidige utvidelser

Vi kan gjøre modellen mer realistisk ved å:
- Bruke `double voltage` i stedet for `bool state`
- Definere terskelverdier for HIGH og LOW (f.eks. 0.8V / 2.0V)
- Simulere støy, jitter eller forsinkelse i signalet
- La komponenter måle **tid over terskel**: f.eks. `hasBeenHighFor(ms)`

Eksempel:

```cpp
bool Clock::isHigh() const {
    return voltage > 4.0;
}
