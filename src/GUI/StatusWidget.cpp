#include "StatusWidget.h"

StatusWidget::StatusWidget(CPU6502* cpu)
{
    m_cpu = cpu;
}

void StatusWidget::render()
{
    if (!m_show)
        return;

    if (!begin("CPU")) {
        end();
        return;
    }

    ImGui::Text("Status: ");
    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::N)), "N");

    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::V)), "V");

    ImGui::SameLine();
    ImGui::TextColored(status_color(true), "-");

    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::B)), "B");

    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::D)), "D");

    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::I)), "I");

    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::Z)), "Z");

    ImGui::SameLine();
    ImGui::TextColored(status_color(m_cpu->get_flag(Flags::C)), "C");

    ImGui::Dummy(ImVec2(0.0f, 1.0f));

    auto a = m_cpu->registers().a;
    auto x = m_cpu->registers().x;
    auto y = m_cpu->registers().y;
    auto pc = m_cpu->registers().pc;
    auto s = m_cpu->registers().s;

    ImGui::Text("A: 0x%02X [%d]", a, static_cast<unsigned>(a));
    ImGui::Text("X: 0x%02X [%d]", x, static_cast<unsigned>(x));
    ImGui::Text("Y: 0x%02X [%d]", y, static_cast<unsigned>(y));

    ImGui::Dummy(ImVec2(0.0f, 1.0f));

    ImGui::Text("Program Counter: 0x%02X", pc);
    ImGui::Text("Stack Pointer:   0x%02X", s);

    ImGui::Dummy(ImVec2(0.0f, 1.0f));

    ImGui::Text("Cycles: %d", m_cpu->cycles());

    end();
}

ImVec4 StatusWidget::status_color(bool status)
{
    ImVec4 set = {.0f, 1.f, .0f, 1.f};
    ImVec4 cleared = {1.f, .0f, .0f, 1.f};
    return (status) ? set : cleared;
}
