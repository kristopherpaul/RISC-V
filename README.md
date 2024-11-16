# RISC-V Emulator

This repository contains a lightweight RISC-V emulator written in C. This emulator aims to simulate a RISC-V CPU and its associated components, enabling the execution of programs built for the RISC-V architecture.

The emulator is built to support specific RISC-V instructions and hardware components necessary for running operating systems like `xv6`.

## Features

- **RISC-V Instruction Set**: Supports RV64I base integer instructions, parts of RV64M (integer multiplication/division), and RV64A (atomic operations).
- **Hardware Simulation**:
  - Central Processing Unit (CPU) &#x2611;
  - Memory and System Bus &#x2611;
  - Control and Status Registers (CSR) &#x2611;
  - Exceptions, PLIC, and CLINT &#x2611;
  - UART and Virtio for device emulation &#x2611;
  - Virtual memory system &#x2612;

## Prerequisites

Ensure you have the following installed:
- GCC (GNU Compiler Collection)
- Make (build automation tool)

## Installation and Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/kristopherpaul/RISC-V.git
   cd RISC-V
   ```

2. Build the emulator:
   ```bash
   make
   ```

3. Run the emulator:
   ```bash
   ./main.exe
   ```

4. Run custom tests:
   ```bash
   make test
   ```

5. Run specific benchmark test cases (e.g., `rv64u`):
   ```bash
   make test_rv64u
   ```

6. Clean build files:
   ```bash
   make clean
   ```

## Project Structure

- **`main.c`**: Entry point for the emulator.
- **`cpu.c`**: Core CPU simulation logic.
- **`trap.c`**: Trap handling for exceptions and interrupts.
- **`clint.c`**: Core Local Interruptor simulation.
- **`dram.c`**: DRAM (memory) emulation.
- **`plic.c`**: Platform-Level Interrupt Controller simulation.
- **`uart.c`**: UART (Universal Asynchronous Receiver-Transmitter) simulation.
- **`bus.c`**: System bus handling for memory and devices.
- **`virtio.c`**: Virtio device support.
- **`test.c`**: Testing utility to ensure correct implementation.

## Authors

<a href="https://github.com/kristopherpaul/risc-v/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=kristopherpaul/risc-v" />
</a>

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests to enhance the emulator or fix bugs.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT). See the LICENSE file for details.
