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
        ImGuiTableFlags_SizingStretchProp | 
        ImGuiTableFlags_ScrollY;

    // TODO: Highlight rows on hover

    if (ImGui::BeginTable("instructions", 3, m_flags)) {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Instruction");
        ImGui::TableSetupColumn("Mode");
        ImGui::TableHeadersRow();

        auto pc = m_cpu->registers().pc;
        bool on_pc;

        for (auto const& [key, value] : m_cpu->disassemble(pc - 26, pc + 30)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            on_pc = hex(pc, 4) == value.address;
            ImGui::Selectable(value.address.c_str(), on_pc, ImGuiSelectableFlags_SpanAllColumns);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(value.instruction.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(value.mode.c_str());
        }

        ImGui::EndTable();
    }

    end();
}
