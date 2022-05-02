#include "Bus.h"
#include "PPU.h"
#include "Cartridge.h"
#include <cstring>
#include <sstream>

Bus::Bus()
{
    cpu_ram.fill(0x00);
}

void Bus::load_program_from_string(const char* program)
{
    std::stringstream stream;
    stream << program;
    u16 offset = 0x8000;
    while (!stream.eof()) {
        std::string b;
        stream >> b;
        cpu_ram[offset++] = (u8)std::stoul(b, nullptr, 16);
    }
    cpu_write(0xFFFC, 0x00);
    cpu_write(0xFFFD, 0x80);
}

u8 Bus::cpu_read(u16 address)
{
    u8 data = 0x00;

    if (m_cartridge->cpu_read(address, data)) {

    }

    else if (address <= 0x1FFF)
        data = cpu_ram[address & 0x07FF];

    else if (address >= 0x2000 && address <= 0x3FFF)
        data = m_ppu->cpu_read(address & 0x0007);

    return data;
}

void Bus::cpu_write(u16 address, u8 value)
{
    if (m_cartridge->cpu_write(address, value)) {

    }
    else if (address <= 0x1FFF)
        cpu_ram[address & 0x07FF] = value;
    
    else if (address >= 0x2000 && address <= 0x3FFF)
        m_ppu->cpu_write(address & 0x0007, value);
}

void Bus::insert_cartridge(Cartridge* cartridge)
{
    m_cartridge = cartridge;
}
