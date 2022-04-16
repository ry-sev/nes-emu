#pragma once
#include "Widget.h"
#include "Bus.h"

class Bus;

class RamWidget : public Widget {

public:
    RamWidget(Bus* bus);
    ~RamWidget() = default;
    void render() override;

private:
    Bus* m_bus;
    ImGuiTableFlags m_flags;
    void create_table(const char* table_name, u16 address, u32 rows);
};
