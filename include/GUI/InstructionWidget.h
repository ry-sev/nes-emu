#pragma once

#include "CPU.h"
#include "Widget.h"
#include <map>
#include <memory>

class NES;

class InstructionWidget : public Widget {

public:
    InstructionWidget(std::shared_ptr<NES> nes);
    ~InstructionWidget() = default;
    void render() override;

private:
    std::shared_ptr<NES> m_nes = nullptr;
    ImGuiTableFlags m_flags;
    std::map<u16, InstructionStrings> m_disassembly;
    bool m_paused = true;
};
