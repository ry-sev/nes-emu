#include "PPU.h"
#include "Cartridge.h"

PPU::PPU()
{
    // https://www.nesdev.org/wiki/PPU_palettes
	m_pallete[0x00] = 0x545454;
	m_pallete[0x01] = 0x001E74;
	m_pallete[0x02] = 0x081090;
	m_pallete[0x03] = 0x30008A;
	m_pallete[0x04] = 0x440064;
	m_pallete[0x05] = 0x5C0030;
	m_pallete[0x06] = 0x540400;
	m_pallete[0x07] = 0x3C1800;
	m_pallete[0x08] = 0x202A00;
	m_pallete[0x09] = 0x083A00;
	m_pallete[0x0A] = 0x004000;
	m_pallete[0x0B] = 0x003C00;
	m_pallete[0x0C] = 0x00323C;
	m_pallete[0x0D] = 0x000000;
	m_pallete[0x0E] = 0x000000;
	m_pallete[0x0F] = 0x000000;

	m_pallete[0x10] = 0x989698;
	m_pallete[0x11] = 0x084CC4;
	m_pallete[0x12] = 0x3032EC;
	m_pallete[0x13] = 0x5C1EE4;
	m_pallete[0x14] = 0x8814B0;
	m_pallete[0x15] = 0xA01464;
	m_pallete[0x16] = 0x982220;
	m_pallete[0x17] = 0x783C00;
	m_pallete[0x18] = 0x545A00;
	m_pallete[0x19] = 0x287200;
	m_pallete[0x1A] = 0x087C00;
	m_pallete[0x1B] = 0x007628;
	m_pallete[0x1C] = 0x006678;
	m_pallete[0x1D] = 0x000000;
	m_pallete[0x1E] = 0x000000;
	m_pallete[0x1F] = 0x000000;

	m_pallete[0x20] = 0xECEEEC;
	m_pallete[0x21] = 0x4C9AEC;
	m_pallete[0x22] = 0x787CEC;
	m_pallete[0x23] = 0xB062EC;
	m_pallete[0x24] = 0xE454EC;
	m_pallete[0x25] = 0xEC58B4;
	m_pallete[0x26] = 0xEC6A64;
	m_pallete[0x27] = 0xD48820;
	m_pallete[0x28] = 0xA0AA00;
	m_pallete[0x29] = 0x74C400;
	m_pallete[0x2A] = 0x4CD020;
	m_pallete[0x2B] = 0x38CC6C;
	m_pallete[0x2C] = 0x38B4CC;
	m_pallete[0x2D] = 0x3C3C3C;
	m_pallete[0x2E] = 0x000000;
	m_pallete[0x2F] = 0x000000;

	m_pallete[0x30] = 0xECEEEC;
	m_pallete[0x31] = 0xA8CCEC;
	m_pallete[0x32] = 0xBCBCEC;
	m_pallete[0x33] = 0xD4B2EC;
	m_pallete[0x34] = 0xECAEEC;
	m_pallete[0x35] = 0xECAED4;
	m_pallete[0x36] = 0xECB4B0;
	m_pallete[0x37] = 0xE4C490;
	m_pallete[0x38] = 0xCCD278;
	m_pallete[0x39] = 0xB4DE78;
	m_pallete[0x3A] = 0xA8E290;
	m_pallete[0x3B] = 0x98E2B4;
	m_pallete[0x3C] = 0xA0D6E4;
	m_pallete[0x3D] = 0xA0A2A0;
	m_pallete[0x3E] = 0x000000;
	m_pallete[0x3F] = 0x000000;
}

void PPU::insert_cartridge(Cartridge* cartridge)
{
    m_cartridge = cartridge;
}

void PPU::clock()
{
    m_cycles++;
}

int PPU::color_from_pallete(u8 pallete, u8 pixel)
{
    auto value = ppu_read(0x3F00 + (pallete << 2) + pixel) & 0x3F;
    return m_pallete[value];
}

u8 PPU::cpu_read(u16 address)
{
    u8 data = 0x00;
    m_ppu_mask.greyscale = 0x00;

    switch (address) {
        case 0x0000: // control
            break;
        case 0x0001: // mask
            break;
        case 0x0002: // status
            m_ppu_status.vertical_blank = 1; // temp
            data = (m_ppu_status.reg & 0xE0) | (m_ppu_data_buffer & 0x1F);
            m_ppu_status.vertical_blank = 0;
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
            break;
        default: break;
    }

    return data;
}

void PPU::cpu_write(u16 address, u8 value)
{
    switch (address) {
        case 0x0000: // control
            m_ppu_control.reg = value;
            break;
        case 0x0001: // mask
            m_ppu_mask.reg = value;
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
            if (m_address_latch == 0) {
                m_ppu_address = (m_ppu_address & 0x00FF) | (value << 8);
                m_address_latch = 1;
            }
            else {
                m_ppu_address = (m_ppu_address & 0xFF00) | value;
                m_address_latch = 0;
            }
            break;
        case 0x0007: // PPU data
            
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
    else if (address >= 0x0000 && address <= 0x1FFF) {
        data = m_pattern_table[(address & 0x1000) >> 12][address & 0x0FFF];
    }
    else if (address >= 0x2000 && address <= 0x3EFF) {

    }
    else if (address >= 0x3F00 && address <= 0x3FFF) {
        address &= 0x001F;
        if (address == 0x0010) address = 0x0000;
        if (address == 0x0014) address = 0x0004;
        if (address == 0x0018) address = 0x0008;
        if (address == 0x001C) address = 0x000C;
        data = m_pallete_table[address];
    }

    return data;
}

void PPU::ppu_write(u16 address, u8 value)
{
    address &= 0x3FFF;
    if (m_cartridge->ppu_write(address, value)) {

    }
    else if (address >= 0x0000 && address <= 0x1FFF) {
         m_pattern_table[(address & 0x1000) >> 12][address & 0x0FFF] = value;
    }
    else if (address >= 0x2000 && address <= 0x3EFF) {

    }
    else if (address >= 0x3F00 && address <= 0x3FFF) {
        address &= 0x001F;
        if (address == 0x0010) address = 0x0000;
        if (address == 0x0014) address = 0x0004;
        if (address == 0x0018) address = 0x0008;
        if (address == 0x001C) address = 0x000C;
        m_pallete_table[address] = value;
    }
    return;
}
