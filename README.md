# 8x8x8 RGB LED Cube

This project is a fully functional **8×8×8 RGB LED cube**, built using **common-anode RGB LEDs** and controlled via BJT transistors. The cube uses **persistence of vision** to display full-color 3D animations by multiplexing through 512 RGB LEDs (1536 channels in total).

---

## Overview

- **Size**: 8×8×8 (512 RGB LEDs)
- **Colors**: Red, Green, Blue (3 channels per LED)
- **Total control lines**: 8 (Z layers) + 8 (Y rows) + 24 (RGB X columns) = 40
- **Transistors**:
  - 192 × NPN (2N3904) – Color control
  - 8 × NPN – Y-axis (emitter rails)
  - 8 × PNP (2N3906) + 8 × NPN – Z-axis (layer power control)

---

## How It Works

The cube is driven by scanning through each of the 64 available 8-LED positions (8 Z layers × 8 Y rows), and lighting them up with RGB values using fast multiplexing. The structure is divided into three control sections:

### X-Axis – Color (RGB) Selection
- 24 control lines: 8 Red, 8 Green, 8 Blue.
- Each line controls a set of 8 NPN transistors (per Z layer).
- These pull the cathodes of the selected color to ground through resistors.

### Y-Axis – Row Selection
- Each row (Y direction) shares emitter rails from the RGB transistors.
- A set of 8 NPN transistors controls these rails.
- Only one row is enabled at a time.

### Z-Axis – Layer Selection
- Each of the 8 vertical layers is powered by a PNP transistor.
- The PNPs are controlled by NPN–PNP pairs for Vcc control.
- Only one layer is powered at a time.

---

## Persistence of Vision

Only one Z-layer and one Y-row are active at any moment. By cycling quickly through all combinations and updating RGB data each time, the viewer perceives a fully lit, animated 3D structure.

> You can light up **24 LEDs (8 RGB units)** per cycle, but with high-speed refresh, the whole cube appears active simultaneously.

---

## Requirements

- Common-anode RGB LEDs (512×)
- 2N3904 NPN transistors (200+)
- 2N3906 PNP transistors (8+)
- Resistors (for current limiting)
- Microcontroller (Arduino Mega, Node MCU)
- Power supply capable of sourcing sufficient current

---

Code Architecture – 8×8×8 RGB LED Cube (Dual-MCU SPI System)
Why Two MCUs?
Controlling a 512 RGB LED cube (8×8×8 = 512 voxels × 3 colors = 1,536 channels) in real-time is computationally intense. This setup offloads responsibilities:

MCU 1 (Master): Handles animation logic, voxel mapping, and prepares raw frame data.

MCU 2 (Slave): Dedicated to real-time LED driving, extremely timing-sensitive, handles multiplexing, SPI decoding, and precise I/O toggling.

This separation ensures animation doesn’t interrupt LED scanning or cause flicker.

Data Protocol Between MCUs (SPI)
SPI is used for fast unidirectional communication: MCU 1 sends precomputed row data to MCU 2.

Data is transferred in chunks: each row of the cube (Z-layer and Y-row) gets a 4-5 byte packet:

Z-index

Y-index

R[8 bits], G[8 bits], B[8 bits] → represent the 8 LEDs along the X-axis.

This allows for full row update in a single transfer, with low latency and synchronization.

MCU 2 – Driving Code Breakdown (Performance-Oriented)
Real-Time Framebuffering with Interrupts
SPI interrupt is heavily optimized to read the incoming frame data and store it immediately into RAM buffers.

When a full row (Z×Y) is received, it’s latched into the display output routine.

Ultra-fast Output Using digitalWriteFast.h
You’re not using loops or abstractions here. Each of the 1,536 possible LED states is manually unrolled.

Code uses explicit digitalWriteFast() calls for each possible LED transistor combination:

cpp
if (r & (1 << 0)) digitalWriteFast(R0_PIN, HIGH);
...
This avoids function call overhead and ensures deterministic timing—no delay, no jitter, crucial for persistence of vision.

Multiplexing Strategy
Only one Y-row and Z-layer is active at a time.

MCU 2 activates a layer (Z) and a row (Y) by toggling the appropriate transistor drivers.

Then it pushes RGB values to the 24 output pins driving the BJT arrays along X-axis.

The sequence is repeated at ~1000 Hz or more, fast enough for human eyes to perceive a stable 3D image.

Why So Many Lines of Code?
Because MCU 2 must:

Control 1,536 transistors explicitly.

Avoid all loops, dynamic indexing, and indirect addressing (too slow).

Map every physical LED to its address in time and space — each diode is manually addressed in the code.

This is the price of high performance on an 8-bit microcontroller:

No DMA.

No framebuffer shifting.

Just control optimized for interrupt latency and memory predictability.

Summary of Code Flow
MCU 1 (Master):
Runs animation logic (procedural or lookup-table based).

Sends rows one by one via SPI: [Z][Y][R][G][B].

MCU 2 (Slave):
SPI interrupt receives and buffers the row data.

Display ISR or main loop:

Selects one Z-layer and one Y-row.

Writes 24 bits to output pins (8×R, 8×G, 8×B).

Moves to the next row.

This repeats 64 times per full cube refresh.
