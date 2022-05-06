#include "PatternTableWidget.h"
#include "PPU.h"
#include "Image.h"
#include <sstream>

PatternTableWidget::PatternTableWidget(PPU* ppu, u8 table_id)
{
    m_ppu = ppu;
    m_table_id = table_id;
    m_pallete_id = 0x00;

    std::stringstream fmt;
    fmt << "PPU Pattern Table " << std::to_string(m_table_id);

    m_window_name = fmt.str();
}

void PatternTableWidget::render()
{
    if (!m_show)
        return;

    if (!begin(m_window_name.c_str())) {
        end();
        return;
    }

    auto window_size = ImGui::GetContentRegionAvail();
    if (window_size.x < window_size.y)
        m_pattern_table.scale_width(window_size.x);
    else
        m_pattern_table.scale_height(window_size.y);

    m_pattern_table.render();
    update();

    end();
}

void PatternTableWidget::update()
{
    if (m_show) {
        update_pattern_table();
        m_pattern_table.update();
    }
}

void PatternTableWidget::update_pattern_table()
{
    for (u16 tile_y = 0; tile_y < 16; tile_y++) {
        
        for (u16 tile_x = 0; tile_x < 16; tile_x++) {

            auto offset = tile_y * 256 + tile_x * 16;

            for (u16 row = 0; row < 8; row++) {

                u8 tile_low_byte = m_ppu->ppu_read(m_table_id * 0x1000 + offset + row + 0x0000);
                u8 tile_high_byte = m_ppu->ppu_read(m_table_id * 0x1000 + offset + row + 0x0008);

                for (u16 column = 0; column < 8; column++) {

                    u8 pixel = (tile_low_byte & 0x01) + (tile_high_byte & 0x01);

                    tile_low_byte >>= 1;
                    tile_high_byte >>= 1;

                    m_pattern_table.set_pixel(
                        tile_x * 8 + (7 - column),
                        tile_y * 8 + row,
                        m_ppu->color_from_pallete(m_pallete_id, pixel)
                    );
                }
            }
        }
    }
}
