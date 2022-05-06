#pragma once

#include "CPU.h"
#include "Widget.h"
#include <map>

class NES;

class InstructionWidget : public Widget {

public:
    InstructionWidget(NES* nes);
    ~InstructionWidget() = default;
    void render() override;

private:
    NES* m_nes;
    ImGuiTableFlags m_flags;
    std::map<u16, InstructionStrings> m_disassembly;
    bool m_paused = false;
};
