#include "ScreenWidget.h"

ScreenWidget::ScreenWidget()
{

}

void ScreenWidget::render()
{
    if (!m_show)
        return;

    if (!begin("Screen")) {
        end();
        return;
    }

    m_draw_list = ImGui::GetWindowDrawList();

    end();
}
