#pragma once
#include "Widget.h"
#include "CPU.h"

class CPU6502;

class StatusWidget : public Widget {

public:
    StatusWidget(CPU6502* cpu);
    ~StatusWidget() = default;
    void render() override;

private:
    CPU6502* m_cpu;
    const char* m_window_name = "CPU Satus";
    ImVec4 status_color(bool status);
};
