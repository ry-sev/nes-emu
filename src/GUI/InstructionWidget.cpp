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

    end();
}
