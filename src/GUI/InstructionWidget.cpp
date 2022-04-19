#include "InstructionWidget.h"

InstructionWidget::InstructionWidget(CPU6502* cpu)
{
    m_cpu = cpu;
    m_disassembly = m_cpu->disassemble(0x0000, 0xFFFF);
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

    if (ImGui::BeginTable("instructions", 3, m_flags)) {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Instruction");
        ImGui::TableSetupColumn("Mode");
        ImGui::TableHeadersRow();

        auto pc = m_cpu->registers().pc;
        bool on_pc;

        auto it = m_disassembly.find(pc);

        if (it != m_disassembly.end()) {
            for (size_t row = 0; row < 26; row++, it++) {
                if (it != m_disassembly.end()) {
                    ImGui::TableNextRow(0, 15.0f);
                    ImGui::TableSetColumnIndex(0);
                    on_pc = hex(pc, 4) == (*it).second.address.c_str();
                    ImGui::Selectable((*it).second.address.c_str(), on_pc, ImGuiSelectableFlags_SpanAllColumns);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted((*it).second.instruction.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted((*it).second.mode.c_str());
                }
            }
        }
        ImGui::EndTable();
    }
    end();
}
