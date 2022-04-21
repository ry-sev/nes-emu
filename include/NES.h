#pragma once

#include "CPU.h"
#include "Bus.h"
#include "PPU.h"

class Cartridge;

class NES {

public:
    NES();
    ~NES() = default;

    void insert_cartridge(Cartridge* cartridge);
    void reset();
    void clock();

    CPU6502* cpu() { return &m_cpu; }
    Bus* bus() { return &m_bus; }
    PPU* ppu() { return &m_ppu; }
    const u32& system_clock_count() const { return m_system_clock_count; }

private:
    Cartridge* m_cartridge = nullptr;
    CPU6502 m_cpu;
    PPU m_ppu;
    Bus m_bus;

    u32 m_system_clock_count = 0;
};
