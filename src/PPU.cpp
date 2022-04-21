#include "PPU.h"

PPU::PPU()
{

}

void PPU::insert_cartridge(Cartridge* cartridge)
{
    m_cartridge = cartridge;
}

void PPU::clock()
{
    m_cycles++;
}

u8 PPU::cpu_read(u16 address)
{
    u8 data = 0x00;

    switch (address) {
        case 0x0000: break; // control
        case 0x0001: break; // mask
        case 0x0002: break; // status
        case 0x0003: break; // OAM address
        case 0x0004: break; // OAM data
        case 0x0005: break; // scroll
        case 0x0006: break; // PPU address
        case 0x0007: break; // PPU data
        default: break;
    }

    return data;
}

void PPU::cpu_write(u16 address, u8 value)
{
    switch (address) {
        case 0x0000: break; // control
        case 0x0001: break; // mask
        case 0x0002: break; // status
        case 0x0003: break; // OAM address
        case 0x0004: break; // OAM data
        case 0x0005: break; // scroll
        case 0x0006: break; // PPU address
        case 0x0007: break; // PPU data
        default: break;
    }

    return;
}

u8 PPU::ppu_read(u16 address)
{
    u8 data = 0x00;
    address &= 0x3FFF;
    return data;
}

void PPU::ppu_write(u16 address, u8 value)
{
    address &= 0x3FFF;
    return;
}
