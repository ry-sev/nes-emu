#pragma once
#include <array>

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

    void insert_cartridge(Cartridge* cartridge);

private:
    Cartridge* m_cartridge;
    CPU6502* m_cpu;
    PPU* m_ppu; 
};
