# VM8

En virtuell 8-bit datamaskin bygget fra bunnen av i C++, inspirert av Ben Eaters SAP-1.

Målet er å bygge en komplett maskin fra D-flipflop og opp – gjennom registere, ALU,
mikrokode og instruksjonssett – og til slutt implementere `eval` i hardware.

## Filosofi

Hver komponent implementerer et felles grensesnitt (`Component`) med innganger,
utganger og respons på klokkekant. Virtuelle komponenter kan byttes ut med ekte
hardware via proxy-klasser – maskinen ser ingen forskjell.

## Struktur
src/core/         – Component-grensesnitt, klokke, signalbus, maskin
src/components/   – Virtuelle hardware-komponenter (flipflop, register, ALU, RAM)
src/proxy/        – Hardware-proxyer (USB, GPIO, atomklokke)
src/frontend/     – WebSocket-server og JSON-protokoll
src/programs/     – Maskinkode-programmer
frontend/         – HTML-frontpanel
docs/             – Protokoll, arkitektur og instruksjonssett

## Inspirasjon

- [Ben Eater – 8-bit breadboard computer](https://eater.net/8bit)
- [SECD-maskinen (Landin, 1964)](https://en.wikipedia.org/wiki/SECD_machine)
