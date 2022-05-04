#pragma once
#include "Widget.h"

class PPU;

class PatternTableWidget : public Widget {

public:
    PatternTableWidget(PPU* ppu);
    ~PatternTableWidget() = default;
    void render() override;

private:
    PPU* m_ppu;
};
