#pragma once

#include "CPU.h"
#include "Bus.h"
#include "PPU.h"
#include <memory>

class Cartridge;

class NES {

public:
    NES();
    ~NES() = default;

    void insert_cartridge(const std::shared_ptr<Cartridge> cartridge);
    void reset();
    void clock();

    std::shared_ptr<CPU6502> cpu() { return m_cpu; }
    std::shared_ptr<Bus> bus() { return m_bus; }
    std::shared_ptr<PPU> ppu() { return m_ppu; }
    const u32& system_clock_count() const { return m_system_clock_count; }
    void write_controller_state(u32 controller_id, u8 data);
    void clear_controller_state(u32 controller_id);

private:
    std::shared_ptr<CPU6502> m_cpu;
    std::shared_ptr<PPU> m_ppu;
    std::shared_ptr<Bus> m_bus;

    u32 m_system_clock_count = 0;
};
