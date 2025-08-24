# Pipelined Multicore Simulator – Computer Architecture Project

This repository contains the implementation of a **pipelined multicore simulator** with MESI-based cache coherence, developed as part of a Computer Architecture course project.

---

## 📋 Overview
- **Architecture:**  
  - Multi-core system with a **shared bus**.  
  - Each core has private caches with coherence maintained by the **MESI protocol**.  
  - Round-robin bus arbitration.  
  - Cycle-accurate simulation in C.

- **Features:**  
  - Instruction/data memory handling.  
  - Trace generation (`coreXtrace.txt`, `bustrace.txt`).  
  - Performance statistics (`statsX.txt`, `cycles`).  
  - Support for multiple workloads (serial/parallel/vectorized).

---

## 📂 Repository Contents
- **`simulator/`** – C implementation of the simulator:  
  - `sim.c`, `bus.c`, `core.c`, `mesi.c`, etc.  
  - Implements multicore pipeline behavior, bus arbitration, MESI, and memory I/O.  
  - Visual Studio project files (`.sln`, `.vcxproj`) included.  

- **`assembly programs/`** – Example workloads:  
  - **`addparallel/`** – vector addition with multiple cores.  
  - **`addserial/`** – serial addition across cores.  
  - **`counter/`** – multicore counter increment program.  
  Each folder includes:
    - `.asm` program(s).  
    - Input memory (`imemX.txt`, `memin.txt`, `dsramX.txt`, `tsramX.txt`).  
    - Simulation outputs (`trace.txt`, `regoutX.txt`, `statsX.txt`, `bustrace.txt`).  
    - Executable (`sim.exe`) for quick run.

- **`project report.pdf`** – detailed written project report.  
- **`דרישות הפרויקט.pdf`** – original project requirements.  
- **`README.md`** – this document.

---

## ▶️ How to Build and Run
1. Navigate to `simulator/`.  
2. Compile all C source files (or open the `.sln` in Visual Studio).  
3. Place the required input memory files (`imemX.txt`, `dsramX.txt`, etc.) into the execution folder.  
4. Run the simulator to produce traces and stats.  
5. Compare outputs with reference results in `assembly programs/`.

---

## 🎯 Purpose
This project demonstrates the design and simulation of a **pipelined multicore processor** with MESI cache coherence, focusing on:  
- Pipeline execution across multiple cores.  
- Memory hierarchy and bus arbitration.  
- Verification of correctness and performance under different workloads.

---

## Authors:
- Yarin Koren
- Jonathan Peled
- Yohai Shiloh
