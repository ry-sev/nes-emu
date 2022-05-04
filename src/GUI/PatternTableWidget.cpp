#include "PatternTableWidget.h"

PatternTableWidget::PatternTableWidget(PPU* ppu)
{
    m_ppu = ppu;
}

void PatternTableWidget::render()
{
    if (!m_show)
        return;

    if (!begin("PPU Pattern Table")) {
        end();
        return;
    }

    end();
}
