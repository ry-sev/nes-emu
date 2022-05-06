#pragma once
#include "Widget.h"
#include "Image.h"

class PPU;

class PatternTableWidget : public Widget {

public:
    PatternTableWidget(PPU* ppu, u8 table_id);
    ~PatternTableWidget() = default;
    
    void render() override;
    void update();
    u8 pallete_id() const { return m_pallete_id; }
    void set_pallete_id(u8 id) { m_pallete_id = id; }

private:
    PPU* m_ppu;
    u8 m_table_id;
    u8 m_pallete_id;
    Image m_pattern_table = Image(128, 128);
    std::string m_window_name;

    void update_pattern_table();
};
