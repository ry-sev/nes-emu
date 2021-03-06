#pragma once
#include "Widget.h"
#include "Image.h"
#include <memory>

class PPU;

class PatternTableWidget : public Widget {

public:
    PatternTableWidget(std::shared_ptr<PPU> ppu, u8 table_id);
    ~PatternTableWidget() = default;
    
    void render() override;
    void update();
    u8 palette_id() const { return m_palette_id; }
    void set_palette_id(u8 id) { m_palette_id = id; }

private:
    std::shared_ptr<PPU> m_ppu;
    u8 m_table_id;
    u8 m_palette_id;
    Image m_pattern_table = Image(128, 128, HOVER);
    std::string m_window_name;

    void update_pattern_table();
};
