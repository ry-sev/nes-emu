#pragma once

class Cartridge;

class PPU {

public:
    PPU();
    ~PPU() = default;

    u8 cpu_read(u16 address);
    void cpu_write(u16 address, u8 value);
    u8 ppu_read(u16 address);
    void ppu_write(u16 address, u8 value);

    void insert_cartridge(Cartridge* cartridge);
    void clock();
    const u16& cycles() const { return m_cycles; }

private:
    Cartridge* m_cartridge;
    u16 m_cycles = 0;

};
