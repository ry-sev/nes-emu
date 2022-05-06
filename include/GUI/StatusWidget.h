#pragma once
#include "Widget.h"
#include <memory>

class CPU6502;

class StatusWidget : public Widget {

public:
    StatusWidget(const std::shared_ptr<CPU6502> cpu);
    ~StatusWidget() = default;
    void render() override;

private:
    std::shared_ptr<CPU6502> m_cpu;
    ImVec4 status_color(bool status);
};
