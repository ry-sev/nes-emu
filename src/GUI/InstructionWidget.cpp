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

    end();
}
