#include "PPU.h"
#include "Cartridge.h"
#include <cassert>

PPU::PPU()
{
    m_screen = new u32[SCREEN_WIDTH * SCREEN_HEIGHT];
}

PPU::~PPU()
{
    delete[] m_screen;
}

void PPU::insert_cartridge(std::shared_ptr<Cartridge> cartridge)
{
    m_cartridge = cartridge;
}

void PPU::clock()
{
    if (m_scan_line > -1 && m_scan_line < 240) {
        if (m_scan_line == -1 && m_cycles == 1)
            m_registers.status.vertical_blank = 0;

        if ((m_cycles >= 2 && m_cycles < 258) || (m_cycles >= 321 && m_cycles < 338)) {

            update_shifters();

            switch ((m_cycles - 1) % 8) {
                case 0:
                    load_background_shifters();
                    m_background.next_tile_id = ppu_read(0x2000 | (m_registers.vram_address.byte & 0x0FFF));
                    break;
                case 2:
                    m_background.next_tile_attribute =
                        ppu_read(
                            0x23C0
                            | (m_registers.vram_address.nametable_y << 11)
                            | (m_registers.vram_address.nametable_x << 10)
                            | ((m_registers.vram_address.coarse_y >> 2) << 3)
                            | (m_registers.vram_address.coarse_x >> 2)
                        );
                    if (m_registers.vram_address.coarse_y & 0x02)
                        m_background.next_tile_attribute >>= 4;
                    if (m_registers.vram_address.coarse_x & 0x02)
                        m_background.next_tile_attribute >>= 2;
                    m_background.next_tile_attribute &= 0x03;
                    break;
                case 4:
                    m_background.next_tile_low_byte = ppu_read(
                        (m_registers.control.pattern_background << 12)
                        + ((u16)m_background.next_tile_id << 4)
                        + (m_registers.vram_address.fine_y)
                        + 0
                    );
                    break;
                case 6:
                    m_background.next_tile_high_byte = ppu_read(
                        (m_registers.control.pattern_background << 12)
                        + ((u16)m_background.next_tile_id << 4)
                        + (m_registers.vram_address.fine_y)
                        + 8
                    );
                    break;
                case 7:
                    increment_scroll_x();
                    break;
                default: break;
            }
        }

        if (m_cycles == 256)
            increment_scroll_y();

        if (m_cycles == 257)
            transfer_address_x();

        if (m_scan_line == -1 && m_cycles >= 280 && m_cycles < 305)
            transfer_address_y();
    }

    if (m_scan_line == 240) {}

    if (m_scan_line == 241 && m_cycles == 1) {
        m_registers.status.vertical_blank = 1;
        if (m_registers.control.enable_nmi)
            m_nmi = true;
    }

    auto [pixel, palette] = current_pixel_to_render();

    const auto x = static_cast<i32>(m_cycles - 1);
    const auto y = static_cast<i32>(m_scan_line);
    const auto color = color_from_palette(palette, pixel);

    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
        m_screen[(y * SCREEN_WIDTH) + x] = color;

    m_cycles++;
    if (m_cycles >= 341) {
        m_cycles = 0;
        m_scan_line++;
        if (m_scan_line > 261) {
            m_scan_line = -1;
            m_frame_complete = true;
        }
    }
}

u32 PPU::color_from_palette(u8 palette, u8 pixel)
{
    assert(palette == 0 || palette == 1);
    auto value = ppu_read(0x3F00 + (palette << 2) + pixel) & 0x3F;
    return m_memory_map.palette[value];
}

std::pair<u8, u8> PPU::current_pixel_to_render()
{
    u8 background_pixel = 0x00;
    u8 background_palette = 0x00;

    if (m_registers.mask.show_background) {
        u16 bit_mux = 0x8000 >> m_fine_x;

        u8 plane_0_pixel = (m_background.shifter_pattern_low_byte & bit_mux) > 0;
        u8 plane_1_pixel = (m_background.shifter_pattern_high_byte & bit_mux) > 0;
        background_pixel = (plane_1_pixel << 1) | plane_0_pixel;

        u8 bg_palette_0 = (m_background.shifter_attribute_low_byte & bit_mux) > 0;
        u8 bg_palette_1 = (m_background.shifter_attribute_high_byte & bit_mux) > 0;
        background_pixel = (bg_palette_1 << 1) | bg_palette_0;
    }

    return std::make_pair(background_pixel, background_palette);
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
            m_ppu_data_buffer = ppu_read(m_registers.vram_address.byte);
            if (m_registers.vram_address.byte > 0x3F00) data = m_ppu_data_buffer;
            m_registers.vram_address.byte += (m_registers.control.increment_mode ? 32 : 1);
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
            m_registers.tram_address.nametable_x = m_registers.control.nametable_x;
            m_registers.tram_address.nametable_y = m_registers.control.nametable_y;
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
            if (m_address_latch == 0x00) {
                m_fine_x = value & 0x07;
                m_registers.tram_address.coarse_x = value >> 3;
                m_address_latch = 0x01;
            }
            else {
                m_registers.tram_address.fine_y = value & 0x07;
                m_registers.tram_address.coarse_x = value >> 3;
                m_address_latch = 0x00;
            }
            break;
        case 0x0006: // PPU address
            if (m_address_latch == 0x00) {
                m_registers.tram_address.byte = (m_registers.tram_address.byte & 0x00FF) | (value << 8);
                m_address_latch = 0x01;
            }
            else {
                m_registers.tram_address.byte = (m_registers.tram_address.byte & 0xFF00) | value;
                m_registers.vram_address = m_registers.tram_address;
                m_address_latch = 0x00;
            }
            break;
        case 0x0007: // PPU data
            ppu_write(m_registers.vram_address.byte, value);
            m_registers.vram_address.byte += (m_registers.control.increment_mode ? 32 : 1);
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
        if (m_cartridge->mirror() == Mirror::VERTICAL) {
            if (address >= 0x0000 && address <= 0x03FF)
                data = m_memory_map.nametable[0][address & 0x03FF];
            if (address >= 0x0400 && address <= 0x07FF)
                data = m_memory_map.nametable[1][address & 0x03FF];
            if (address >= 0x0800 && address <= 0x0BFF)
                data = m_memory_map.nametable[0][address & 0x03FF];
            if (address >= 0x0C00 && address <= 0x0FFF)
                data = m_memory_map.nametable[1][address & 0x03FF];
        }
        else if (m_cartridge->mirror() == Mirror::HORIZONTAL) {
            if (address >= 0x0000 && address <= 0x03FF)
                data = m_memory_map.nametable[0][address & 0x03FF];
            if (address >= 0x0400 && address <= 0x07FF)
                data = m_memory_map.nametable[0][address & 0x03FF];
            if (address >= 0x0800 && address <= 0x0BFF)
                data = m_memory_map.nametable[1][address & 0x03FF];
            if (address >= 0x0C00 && address <= 0x0FFF)
                data = m_memory_map.nametable[1][address & 0x03FF];
        }
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
        if (m_cartridge->mirror() == Mirror::VERTICAL) {
            if (address >= 0x0000 && address <= 0x03FF)
                m_memory_map.nametable[0][address & 0x03FF] = value;
            if (address >= 0x0400 && address <= 0x07FF)
                m_memory_map.nametable[1][address & 0x03FF] = value;
            if (address >= 0x0800 && address <= 0x0BFF)
                m_memory_map.nametable[0][address & 0x03FF] = value;
            if (address >= 0x0C00 && address <= 0x0FFF)
                m_memory_map.nametable[1][address & 0x03FF] = value;
        }
        else if (m_cartridge->mirror() == Mirror::HORIZONTAL) {
            if (address >= 0x0000 && address <= 0x03FF)
                m_memory_map.nametable[0][address & 0x03FF] = value;
            if (address >= 0x0400 && address <= 0x07FF)
                m_memory_map.nametable[0][address & 0x03FF] = value;
            if (address >= 0x0800 && address <= 0x0BFF)
                m_memory_map.nametable[1][address & 0x03FF] = value;
            if (address >= 0x0C00 && address <= 0x0FFF)
                m_memory_map.nametable[1][address & 0x03FF] = value;
        }
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

void PPU::increment_scroll_x()
{
    if (m_registers.mask.show_background || m_registers.mask.show_sprites) {
        if (m_registers.vram_address.coarse_x == 31) {
            m_registers.vram_address.coarse_x = 0;
            m_registers.vram_address.nametable_x = ~m_registers.vram_address.nametable_x;
        }
        else
            m_registers.vram_address.coarse_x++;
    }
}

void PPU::increment_scroll_y()
{
    if (m_registers.mask.show_background || m_registers.mask.show_sprites) {
        if (m_registers.vram_address.fine_y < 8)
            m_registers.vram_address.fine_y++;
        else {
            m_registers.vram_address.fine_y = 0;
            if (m_registers.vram_address.coarse_y == 29) {
                m_registers.vram_address.coarse_y = 0;
                m_registers.vram_address.nametable_y = ~m_registers.vram_address.nametable_y;
            }
            else if (m_registers.vram_address.coarse_y == 31)
                m_registers.vram_address.coarse_y = 0;
            else
                m_registers.vram_address.coarse_y++;
        }
    }
}

void PPU::transfer_address_x()
{
    if (m_registers.mask.show_background || m_registers.mask.show_sprites) {
        m_registers.vram_address.nametable_x = m_registers.tram_address.nametable_x;
        m_registers.vram_address.coarse_x = m_registers.tram_address.coarse_x;
    }
}

void PPU::transfer_address_y()
{
    if (m_registers.mask.show_background || m_registers.mask.show_sprites) {
        m_registers.vram_address.fine_y = m_registers.tram_address.fine_y;
        m_registers.vram_address.nametable_y = m_registers.tram_address.nametable_y;
        m_registers.vram_address.coarse_y = m_registers.tram_address.coarse_y;
    }
}

void PPU::load_background_shifters()
{
    m_background.shifter_pattern_low_byte = (m_background.shifter_pattern_low_byte & 0xFF00) | m_background.next_tile_low_byte;

    m_background.shifter_pattern_high_byte = (m_background.shifter_pattern_high_byte & 0xFF00) | m_background.shifter_pattern_high_byte;

    m_background.shifter_attribute_low_byte = (m_background.shifter_attribute_low_byte & 0xFF00) | ((m_background.next_tile_attribute & 0b01) ? 0xFF : 0x00);

    m_background.shifter_attribute_high_byte = (m_background.shifter_attribute_high_byte  & 0xFF00) | ((m_background.next_tile_attribute & 0b01) ? 0xFF : 0x00);
}

void PPU::update_shifters()
{
    if (m_registers.mask.show_background) {
        m_background.shifter_pattern_low_byte <<= 1;
        m_background.shifter_pattern_high_byte <<= 1;
        m_background.shifter_attribute_low_byte <<= 1;
        m_background.shifter_attribute_high_byte <<= 1;
    }
}
