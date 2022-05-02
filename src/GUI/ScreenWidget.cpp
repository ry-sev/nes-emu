#include "ScreenWidget.h"

ScreenWidget::ScreenWidget()
{

}

void ScreenWidget::render()
{
    if (!m_show)
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    begin("Screen");

    m_viewport_width = ImGui::GetContentRegionAvail().x;
    m_viewport_height = ImGui::GetContentRegionAvail().y;

    end();
    ImGui::PopStyleVar();
}
