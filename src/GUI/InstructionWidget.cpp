#include "InstructionWidget.h"
#include "NES.h"

InstructionWidget::InstructionWidget(NES* nes)
{
    m_nes = nes;
    m_disassembly = m_nes->cpu()->disassemble(0x0000, 0xFFFF);
}

void InstructionWidget::render()
{
    if (!m_show)
        return;

    if (!begin("CPU Instructions")) {
        end();
        return;
    }

    const char* button_label_1 = m_paused ? "Resume" : "Pause";

    if (ImGui::Button(button_label_1)) {
        if (m_paused)
            m_paused = false;
        else
            m_paused = true;
    }

    if (!m_paused)
        m_nes->clock();

    ImGui::SameLine();

    if (m_paused) {
        if (ImGui::Button("Step"))
            m_nes->clock();
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Reset"))
        m_nes->reset();

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

        auto pc = m_nes->cpu()->registers().pc;
        bool on_pc;
        auto it = m_disassembly.find(pc);
        auto prev = std::prev(it);

        std::map<u16, InstructionStrings> first_13;

        // FIXME: There has to be a more efficient way to list 26 instructions
        // with the program counter in the middle

        if (prev != m_disassembly.end()) {
            for (size_t row = 0; row < 12; row++) {
                if (prev != m_disassembly.end()) {
                    first_13[prev->first] = prev->second;
                    prev = std::prev(prev);
                }
            }
        }

        for (auto it = first_13.begin(); it != first_13.end(); it++ ) {
            ImGui::TableNextRow(0, 15.0f);
            ImGui::TableSetColumnIndex(0);
            on_pc = hex(pc, 4) == it->second.address.c_str();
            ImGui::Selectable(it->second.address.c_str(), on_pc, ImGuiSelectableFlags_SpanAllColumns);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(it->second.instruction.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(it->second.mode.c_str());
        }

        size_t limit = 26 - first_13.size();

        it = m_disassembly.find(pc);
        if (it != m_disassembly.end()) {
            for (size_t row = 0; row < limit; row++, it++) {
                if (it != m_disassembly.end()) {
                    ImGui::TableNextRow(0, 15.0f);
                    ImGui::TableSetColumnIndex(0);
                    on_pc = hex(pc, 4) == it->second.address.c_str();
                    ImGui::Selectable(it->second.address.c_str(), on_pc, ImGuiSelectableFlags_SpanAllColumns);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(it->second.instruction.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(it->second.mode.c_str());
                }
            }
        }
        ImGui::EndTable();
    }
    end();
}
