#include "MenuWidget.h"

MenuWidget::MenuWidget()
{

}

void MenuWidget::render()
{
    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate
        );
        ImGui::EndMainMenuBar();
    }
}
