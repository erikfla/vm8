# 🧠 Virtuell 8-bit Datamaskin i C++

Et lærings- og utviklingsprosjekt inspirert av Ben Eaters breadboard-datamaskin, med mål om å bygge en komplett virtuell CPU fra bunnen av – og på sikt en enkel eval-baserte virtuell maskin.

---

## 🥅 Første delmål

- Implementere en virtuell klokke med visuell puls (▮/_)
- Lage en enkel CPU-løkke med:
  - RAM (16 adresser, 8-bit)
  - Register: PC, IR, ACC
  - Instruksjoner: LDA, OUT, HLT
- Kjøre testprogram som:
  1. Laster 42 fra adresse 0x01
  2. Skriver det ut med OUT
  3. Stopper med HLT
- Kjøre i terminalen med `step`, `run`, og `halt`-modus

---

## 🔷 Innhold

1. 🎯 Mål og motivasjon
2. 🏗️ Arkitektur
    - CPU-komponenter
    - Instruksjonssett
    - Klokke og kontroll
3. 🧪 Simulering og grensesnitt
    - Terminalbasert kjøring
    - Run/Step/Halt-brytere
    - ASCII-visning av registre og klokkesignal
4. 🧱 Logiske byggesteiner
    - SR-latch i kode
    - RAM-bygging med porter
    - ALU på bitnivå
5. 🗃️ Programlast og assembler
    - Symbolsk assembler-syntaks
    - Parser til maskinkode
6. 🔄 Eval og virtuell maskin
    - Stackbasert tolk
    - Enkle uttrykk (`(+ 2 3)`)
    - Implementere `eval` i egen maskin
7. 🧰 Videreføring og fysisk port
    - Arduino / Fysisk realisering
    - Eventuelt FPGA
8. 📎 Filer og mapper
    - `src/` – Kildekode
    - `docs/` – Diagrammer og forklaringer
    - `assembler/` – Verktøy for maskinkode
    - `programs/` – Ferdige programmer
