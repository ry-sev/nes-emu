#include "InstructionWidget.h"

InstructionWidget::InstructionWidget(CPU6502* cpu)
{
    m_cpu = cpu;
}

void InstructionWidget::render()
{
    if (!m_show)
        return;

    if (!begin("CPU Instructions")) {
        end();
        return;
    }

    if (ImGui::Button("Run")) {
        while (!(m_cpu->registers().a == 0x1e))
            m_cpu->clock();
    }

    ImGui::SameLine();

    if (ImGui::Button("Step"))
        m_cpu->clock();
    
    ImGui::SameLine();
    
    if (ImGui::Button("Reset"))
        m_cpu->reset();

    ImGui::Dummy(ImVec2(0.0f, 1.0f));

    m_flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_NoHostExtendX | 
        ImGuiTableFlags_ScrollY;

    // TODO: Highlight rows on hover

    if (ImGui::BeginTable("instructions", 3, m_flags)) {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Instruction");
        ImGui::TableSetupColumn("Mode");
        ImGui::TableHeadersRow();

        for (auto const& [key, value] : m_cpu->disassemble(0x7FFA, 0xAAAA)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", value.address.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", value.instruction.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", value.mode.c_str());
        }

        ImGui::EndTable();
    }

    end();
}
