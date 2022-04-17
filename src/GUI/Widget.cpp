#include "Widget.h"
#include <imgui.h>

bool Widget::begin(const char* widget_title) {
    // m_window_flags |= ImGuiWindowFlags_NoCollapse;
    // ImGuiWindowFlags_NoMove
    // ImGuiWindowFlags_NoResize
    // ImGuiWindowFlags_NoCollapse
    // ImGuiWindowFlags_NoSavedSettings
    // ImGuiWindowFlags_AlwaysAutoResize
    // ImGuiWindowFlags_NoTitleBar
    // ImGuiWindowFlags_NoScrollbar
    // ImGuiWindowFlags_MenuBar
    // ImGuiWindowFlags_NoNav
    // ImGuiWindowFlags_NoBackground
    // ImGuiWindowFlags_NoBringToFrontOnFocus

    return ImGui::Begin(widget_title, nullptr, m_window_flags);
}

void Widget::end() {
    ImGui::End();
}
