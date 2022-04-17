#include "MenuWidget.h"

MenuWidget::MenuWidget()
{

}

void MenuWidget::render()
{
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::Button("Reset");
        ImGui::EndMainMenuBar();
    }
}
