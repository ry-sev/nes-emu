#include "PPU.h"
#include "Cartridge.h"
#include <cstring>

PPU::PPU()
{
    m_screen = new i32[SCREEN_WIDTH * SCREEN_HEIGHT];
}

PPU::~PPU()
{
    delete[] m_screen;
}

void PPU::insert_cartridge(const std::shared_ptr<Cartridge> cartridge)
{
    m_cartridge = cartridge;
}

void PPU::reset()
{
    m_fine_x = 0x00;
    m_address_latch = 0x00;
    m_ppu_data_buffer = 0x00;
    m_scan_line = 0;
    m_cycles = 0;
    m_background.next_tile_id = 0x00;
    m_background.next_tile_attribute = 0x00;
    m_background.next_tile_low_byte = 0x00;
    m_background.next_tile_high_byte = 0x00;
    m_background.shifter_pattern_low_byte = 0x0000;
    m_background.shifter_pattern_high_byte = 0x0000;
    m_background.shifter_attribute_low_byte = 0x0000;
    m_background.shifter_attribute_high_byte = 0x0000;
    m_registers.status.byte = 0x00;
    m_registers.mask.byte = 0x00;
    m_registers.control.byte = 0x00;
    m_registers.vram_address.byte = 0x00;
    m_registers.tram_address.byte = 0x00;
}

void PPU::clock()
{
    if (m_scan_line >= -1 && m_scan_line < 240) {

        if (m_scan_line == 0 && m_cycles == 0)
            m_cycles = 1;

        if (m_scan_line == -1 && m_cycles == 1) {
            m_registers.status.vertical_blank = 0;
            m_registers.status.sprite_overflow = 0;
            m_registers.status.sprite_zero_hit = 0;

            for (size_t i = 0; i < 8; i++) {
                m_foreground.sprite_shifter_pattern_low_byte[i] = 0;
                m_foreground.sprite_shifter_pattern_high_byte[i] = 0;
            }
        }

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
            
            if (m_cycles == 256)
                increment_scroll_y();

            if (m_cycles == 257) {
                load_background_shifters();
                transfer_address_x();
            }

            if (m_cycles == 338 || m_cycles == 340)
                m_background.next_tile_id = ppu_read(0x2000 | (m_registers.vram_address.byte & 0x0FFF));

            if (m_scan_line == -1 && m_cycles >= 280 && m_cycles < 305)
                transfer_address_y();

            // foreground rendering

            if (m_cycles == 257 && m_scan_line >= 0) {
                // FIXME: Dont use memset
                //std::memset(m_sprite_scan_line, 0xFF, 8 * sizeof(ObjectAttributeEntry));
                m_sprite_count = 0;
                u8 oam_entry = 0;
                m_sprite_zero_hit_possible = false;

                for (u8 i = 0; i < 8; i++) {
                    m_foreground.sprite_shifter_pattern_low_byte[i] = 0;
                    m_foreground.sprite_shifter_pattern_high_byte[i] = 0;
                }

                while (oam_entry < 64 && m_sprite_count < 9) {
                    i16 diff = ((i16)m_scan_line - (i16)m_oam[oam_entry].y_position);
                    if (diff >= 0 && diff < (m_registers.control.sprite_size ? 16 : 8)) {
                        if (m_sprite_count < 8) {
                            if (oam_entry == 0)
                                m_sprite_zero_hit_possible = true;
                            // memcpy(
                            //     &m_sprite_scan_line[m_sprite_count],
                            //     &m_oam[oam_entry],
                            //     sizeof(ObjectAttributeEntry)
                            // );
                            m_oam[oam_entry] = m_sprite_scan_line[m_sprite_count];
                            m_sprite_count++;
                        }
                    }
                    oam_entry++;
                }
                m_registers.status.sprite_overflow = (m_sprite_count > 8);
            }

            if (m_cycles == 340) {
                for (u8 i = 0; i < m_sprite_count; i++) {
                    
                    u8 sprite_pattern_bits_low;
                    u8 sprite_pattern_bits_high;
                    u16 sprite_pattern_address_low;
                    u16 sprite_pattern_address_high;
                    
                    if (!m_registers.control.sprite_size) {
                        // 8x8 sprite mode
                        if (!(m_sprite_scan_line[i].attribute & 0x80)) {
                            sprite_pattern_address_low =
                                (m_registers.control.pattern_sprite << 12)
                                | (m_sprite_scan_line[i].id << 4)
                                | (m_scan_line - m_sprite_scan_line[i].y_position);
                        }
                        else {
                            sprite_pattern_address_low =
                                (m_registers.control.pattern_sprite << 12)
                                | (m_sprite_scan_line[i].id << 4)
                                | (7 - (m_scan_line - m_sprite_scan_line[i].y_position));
                        }
                    }
                    else {
                        // 8x16 sprite mode
                        if (!(m_sprite_scan_line[i].attribute & 0x80)) {
                            // read top half to tile
                            if (m_scan_line - m_sprite_scan_line[i].y_position < 8) {
                                sprite_pattern_address_low =
                                    ((m_sprite_scan_line[i].id & 0x01) << 12)
                                    | ((m_sprite_scan_line[i].id & 0xFE) << 4)
                                    | ((m_scan_line - m_sprite_scan_line[i].y_position) & 0x07);
                            }
                            else {
                                // read bottom half of tile
                                sprite_pattern_address_low =
                                    ((m_sprite_scan_line[i].id & 0x01) << 12)
                                    | (((m_sprite_scan_line[i].id & 0xFE) + 1) << 4)
                                    | ((m_scan_line - m_sprite_scan_line[i].y_position) & 0x07);
                            }
                        }
                        else {
                            if (m_scan_line - m_sprite_scan_line[i].y_position < 8) {
                                sprite_pattern_address_low =
                                    ((m_sprite_scan_line[i].id & 0x01) << 12)
                                    | (((m_sprite_scan_line[i].id & 0xFE) + 1) << 4)
                                    | (7 - ((m_scan_line - m_sprite_scan_line[i].y_position) & 0x07));
                            }
                            else {
                                sprite_pattern_address_low =
                                    ((m_sprite_scan_line[i].id & 0x01) << 12)
                                    | ((m_sprite_scan_line[i].id & 0xFE) << 4)
                                    | (7 -((m_scan_line - m_sprite_scan_line[i].y_position) & 0x07));
                            }
                        }
                    }

                    sprite_pattern_address_high = sprite_pattern_address_low + 8;
                    sprite_pattern_bits_low = ppu_read(sprite_pattern_address_low);
                    sprite_pattern_bits_high = ppu_read(sprite_pattern_address_high);

                    if (m_sprite_scan_line[i].attribute & 0x40) {
                        sprite_pattern_bits_low = flip_byte(sprite_pattern_bits_low);
                        sprite_pattern_bits_high = flip_byte(sprite_pattern_bits_high);
                    }

                    m_foreground.sprite_shifter_pattern_low_byte[i] = sprite_pattern_bits_low;
                    m_foreground.sprite_shifter_pattern_high_byte[i] = sprite_pattern_bits_high;
                }
            }
        }
    }

    if (m_scan_line == 240) {}

    if (m_scan_line == 241 && m_scan_line < 261) {
        if (m_scan_line == 241 && m_cycles == 1) {
            m_registers.status.vertical_blank = 1;
            if (m_registers.control.enable_nmi)
                m_nmi = true;
        }
    }

    auto x = static_cast<i32>(m_cycles - 1);
    auto y = static_cast<i32>(m_scan_line);

    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        auto [pixel, palette] = current_pixel_to_render();
        auto color = color_from_palette(palette, pixel);
        m_screen[(y * SCREEN_WIDTH) + x] = color;
    }

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

i32 PPU::color_from_palette(u8 palette, u8 pixel)
{
    auto value = ppu_read(0x3F00 + (palette << 2) + pixel) & 0x3F;
    return m_memory_map.palette[value];
}

std::pair<u8, u8> PPU::current_pixel_to_render()
{
    u8 bg_pixel = 0x00;
    u8 bg_palette = 0x00;

    if (m_registers.mask.show_background) {
        u16 bit_mux = 0x8000 >> m_fine_x;

        u8 plane_0_pixel = (m_background.shifter_pattern_low_byte & bit_mux) > 0;
        u8 plane_1_pixel = (m_background.shifter_pattern_high_byte & bit_mux) > 0;
        bg_pixel = (plane_1_pixel << 1) | plane_0_pixel;

        u8 bg_palette_0 = (m_background.shifter_attribute_low_byte & bit_mux) > 0;
        u8 bg_palette_1 = (m_background.shifter_attribute_high_byte & bit_mux) > 0;
        bg_palette = (bg_palette_1 << 1) | bg_palette_0;
    }

    u8 fg_pixel = 0x00;
    u8 fg_palette = 0x00;
    u8 fg_priority = 0x00;

    if (m_registers.mask.show_sprites) {
        m_sprite_zero_rendered = false;
        for (u8 i = 0; i < m_sprite_count; i++) {
            if (m_sprite_scan_line[i].x_position == 0) {
                u8 fg_pixel_low = (m_foreground.sprite_shifter_pattern_low_byte[i] & 0x80) > 0;
                u8 fg_pixel_high = (m_foreground.sprite_shifter_pattern_high_byte[i] & 0x80) > 0;
                fg_pixel = (fg_pixel_high << 1) | fg_pixel_low;

                fg_palette = (m_sprite_scan_line[i].attribute & 0x03) + 0x04;
                fg_priority = (m_sprite_scan_line[i].attribute & 0x20) == 0;

                if (fg_pixel != 0) {
                    if (i == 0)
                        m_sprite_zero_rendered = true;
                    break;
                }
            }
        }
    }

    u8 final_pixel = 0x00;
    u8 final_palette = 0x00;

    if (bg_pixel == 0 && fg_pixel == 0) {
        final_pixel = 0x00;
        final_palette = 0x00;
    }
    else if (bg_pixel == 0 && fg_pixel > 0) {
        final_pixel = fg_pixel;
        final_palette = fg_palette;
    }
    else if (bg_pixel > 0 && fg_pixel == 0) {
        final_pixel = bg_pixel;
        final_palette = bg_palette;
    }
    else if (bg_pixel > 0 && fg_pixel > 0) {
        
        final_pixel = (fg_priority) ? fg_pixel : bg_pixel;
        final_palette = (fg_priority) ? fg_palette : bg_palette;

        if (m_sprite_zero_hit_possible && m_sprite_zero_rendered) {
            if (m_registers.mask.show_background_left | m_registers.mask.show_sprites_left) {
                if (m_cycles >= 9 && m_cycles < 258)
                    m_registers.status.sprite_zero_hit = 1;
            }
            else {
                if (m_cycles >= 1 && m_cycles < 258)
                    m_registers.status.sprite_zero_hit = 1;
            }
        }
    }

    return std::make_pair(final_pixel, final_palette);
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
            data = oam_ptr[m_oam_address];
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
            m_oam_address = value;
            break;
        case 0x0004: // OAM data
            oam_ptr[m_oam_address] = value;
            break;
        case 0x0005: // scroll
            if (m_address_latch == 0x00) {
                m_fine_x = value & 0x07;
                m_registers.tram_address.coarse_x = value >> 3;
                m_address_latch = 0x01;
            }
            else {
                m_registers.tram_address.fine_y = value & 0x07;
                m_registers.tram_address.coarse_y = value >> 3;
                m_address_latch = 0x00;
            }
            break;
        case 0x0006: // PPU address
            if (m_address_latch == 0x00) {
                m_registers.tram_address.byte = (u16)((value & 0x3F) << 8 | (m_registers.tram_address.byte & 0x00FF));
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

    if (m_cartridge->ppu_read(address, data)) {}
    
    else if (address <= 0x1FFF)
        data = m_memory_map.pattern_table[(address & 0x1000) >> 12][address & 0x0FFF];
    
    else if (address >= 0x2000 && address <= 0x3EFF) {
        address &= 0x0FFF;
        if (m_cartridge->mirror() == Mirror::VERTICAL) {
            if (address <= 0x03FF)
                data = m_memory_map.nametable[0][address & 0x03FF];
            if (address >= 0x0400 && address <= 0x07FF)
                data = m_memory_map.nametable[1][address & 0x03FF];
            if (address >= 0x0800 && address <= 0x0BFF)
                data = m_memory_map.nametable[0][address & 0x03FF];
            if (address >= 0x0C00 && address <= 0x0FFF)
                data = m_memory_map.nametable[1][address & 0x03FF];
        }
        else if (m_cartridge->mirror() == Mirror::HORIZONTAL) {
            if (address <= 0x03FF)
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
        data = m_memory_map.palette_table[address] & (m_registers.mask.greyscale ? 0x30 : 0x3F);
    }

    return data;
}

void PPU::ppu_write(u16 address, u8 value)
{
    address &= 0x3FFF;
    if (m_cartridge->ppu_write(address, value)) {}
    
    else if (address <= 0x1FFF)
         m_memory_map.pattern_table[(address & 0x1000) >> 12][address & 0x0FFF] = value;
    
    else if (address >= 0x2000 && address <= 0x3EFF) {
        address &= 0x0FFF;
        if (m_cartridge->mirror() == Mirror::VERTICAL) {
            if (address <= 0x03FF)
                m_memory_map.nametable[0][address & 0x03FF] = value;
            if (address >= 0x0400 && address <= 0x07FF)
                m_memory_map.nametable[1][address & 0x03FF] = value;
            if (address >= 0x0800 && address <= 0x0BFF)
                m_memory_map.nametable[0][address & 0x03FF] = value;
            if (address >= 0x0C00 && address <= 0x0FFF)
                m_memory_map.nametable[1][address & 0x03FF] = value;
        }
        else if (m_cartridge->mirror() == Mirror::HORIZONTAL) {
            if (address <= 0x03FF)
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
        if (m_registers.vram_address.fine_y < 7)
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

    m_background.shifter_pattern_high_byte = (m_background.shifter_pattern_high_byte & 0xFF00) | m_background.next_tile_high_byte;

    m_background.shifter_attribute_low_byte = (m_background.shifter_attribute_low_byte & 0xFF00) | ((m_background.next_tile_attribute & 0b01) ? 0xFF : 0x00);

    m_background.shifter_attribute_high_byte = (m_background.shifter_attribute_high_byte  & 0xFF00) | ((m_background.next_tile_attribute & 0b10) ? 0xFF : 0x00);
}

void PPU::update_shifters()
{
    if (m_registers.mask.show_background) {
        m_background.shifter_pattern_low_byte <<= 1;
        m_background.shifter_pattern_high_byte <<= 1;
        m_background.shifter_attribute_low_byte <<= 1;
        m_background.shifter_attribute_high_byte <<= 1;
    }

    if (m_registers.mask.show_sprites && m_cycles >= 1 && m_cycles < 258) {
        for (size_t i = 0; i < m_sprite_count; i++) {
            if (m_sprite_scan_line[i].x_position > 0)
                m_sprite_scan_line[i].x_position--;
            else {
                m_foreground.sprite_shifter_pattern_low_byte[i] <<= 1;
                m_foreground.sprite_shifter_pattern_high_byte[i] <<= 1;
            }
        }
    }
}
