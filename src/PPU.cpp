#include "PPU.h"
#include "Cartridge.h"
#include <cassert>

PPU::PPU()
{

}

void PPU::insert_cartridge(std::shared_ptr<Cartridge> cartridge)
{
    m_cartridge = cartridge;
}

void PPU::clock()
{
    m_cycles++;
}

u32 PPU::color_from_palette(u8 palette, u8 pixel)
{
    assert(palette == 0 || palette == 1);
    auto value = ppu_read(0x3F00 + (palette << 2) + pixel) & 0x3F;
    return m_memory_map.palette[value];
}

u8 PPU::cpu_read(u16 address)
{
    u8 data = 0x00;
    switch (address) {
        case 0x0000: // control
            break;
        case 0x0001: // mask
            break;
        case 0x0002: // status
            m_registers.status.vertical_blank = 1; // temp
            data = (m_registers.status.byte & 0xE0) | (m_ppu_data_buffer & 0x1F);
            m_registers.status.vertical_blank = 0;
            m_address_latch = 0;
            break;
        case 0x0003: // OAM address
            break;
        case 0x0004: // OAM data
            break;
        case 0x0005: // scroll
            break;
        case 0x0006: // PPU address
            break;
        case 0x0007: // PPU data
            data = m_ppu_data_buffer;
            m_ppu_data_buffer = ppu_read(m_ppu_address);
            if (m_ppu_address > 0x3F00) data = m_ppu_data_buffer;
            m_ppu_address++;
            break;
        default: break;
    }
    return data;
}

void PPU::cpu_write(u16 address, u8 value)
{
    switch (address) {
        case 0x0000: // control
            m_registers.control.byte = value;
            break;
        case 0x0001: // mask
            m_registers.mask.byte = value;
            break;
        case 0x0002: // status
            break;
        case 0x0003: // OAM address
            break;
        case 0x0004: // OAM data
            break;
        case 0x0005: // scroll
            break;
        case 0x0006: // PPU address
            if (m_address_latch == 0x00) {
                m_ppu_address = (m_ppu_address & 0x00FF) | (value << 8);
                m_address_latch = 0x01;
            }
            else {
                m_ppu_address = (m_ppu_address & 0xFF00) | value;
                m_address_latch = 0x00;
            }
            break;
        case 0x0007: // PPU data
            ppu_write(m_ppu_address, value);
            m_ppu_address++;
            break;
        default: break;
    }

    return;
}

u8 PPU::ppu_read(u16 address)
{
    u8 data = 0x00;
    address &= 0x3FFF;

    if (m_cartridge->ppu_read(address, data)) {

    }
    else if (address <= 0x1FFF) {
        data = m_memory_map.pattern_table[(address & 0x1000) >> 12][address & 0x0FFF];
    }
    else if (address >= 0x2000 && address <= 0x3EFF) {

    }
    else if (address >= 0x3F00 && address <= 0x3FFF) {
        address &= 0x001F;
        if (address == 0x0010) address = 0x0000;
        if (address == 0x0014) address = 0x0004;
        if (address == 0x0018) address = 0x0008;
        if (address == 0x001C) address = 0x000C;
        data = m_memory_map.palette_table[address];
    }

    return data;
}

void PPU::ppu_write(u16 address, u8 value)
{
    address &= 0x3FFF;
    if (m_cartridge->ppu_write(address, value)) {

    }
    else if (address <= 0x1FFF) {
         m_memory_map.pattern_table[(address & 0x1000) >> 12][address & 0x0FFF] = value;
    }
    else if (address >= 0x2000 && address <= 0x3EFF) {

    }
    else if (address >= 0x3F00 && address <= 0x3FFF) {
        address &= 0x001F;
        if (address == 0x0010) address = 0x0000;
        if (address == 0x0014) address = 0x0004;
        if (address == 0x0018) address = 0x0008;
        if (address == 0x001C) address = 0x000C;
        m_memory_map.palette_table[address] = value;
    }
    return;
}
