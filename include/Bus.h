#pragma once
#include <array>
#include <memory>

#define CPU_RAM_SIZE 0x2048

class Cartridge;
class CPU6502;
class PPU;

class Bus {

public:
    Bus();
    ~Bus() = default;

    std::array<uint8_t, CPU_RAM_SIZE> cpu_ram;

    void load_program_from_string(const char* program);
    u8 cpu_read(u16 address);
    void cpu_write(u16 address, u8 value);

    void connect_to_ppu(std::shared_ptr<PPU> ppu) { m_ppu = ppu; };
    void insert_cartridge(std::shared_ptr<Cartridge> cartridge);

private:
    std::shared_ptr<Cartridge> m_cartridge;
    std::shared_ptr<CPU6502> m_cpu;
    std::shared_ptr<PPU> m_ppu; 
};
