#pragma once
#include "Widget.h"
#include "CPU.h"

class CPU6502;

class InstructionWidget : public Widget {

public:
    InstructionWidget(CPU6502* cpu);
    ~InstructionWidget() = default;
    void render() override;

private:
    CPU6502* m_cpu;
    ImGuiTableFlags m_flags;
    std::map<u16, InstructionStrings> m_disassembly;
};
