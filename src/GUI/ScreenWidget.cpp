#include "ScreenWidget.h"
#include "PPU.h"

ScreenWidget::ScreenWidget(std::shared_ptr<PPU> ppu)
{
    m_ppu = ppu;
}

void ScreenWidget::render()
{
    if (!m_show)
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    begin("Screen");

    auto window_size = ImGui::GetContentRegionAvail();
    if (window_size.x < window_size.y)
        m_screen.scale_width(window_size.x);
    else
        m_screen.scale_height(window_size.y);

    m_screen.render();
    
    m_screen.array_to_image(m_ppu->screen());

    m_screen.update();

    end();
    ImGui::PopStyleVar();
}
