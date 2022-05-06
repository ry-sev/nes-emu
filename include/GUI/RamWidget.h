#pragma once
#include "Widget.h"
#include <memory>

class Bus;

class RamWidget : public Widget {

public:
    RamWidget(const std::shared_ptr<Bus> bus);
    ~RamWidget() = default;
    void render() override;

private:
    std::shared_ptr<Bus> m_bus;
    ImGuiTableFlags m_flags;
    void create_table(const char* table_name, u16 address, u32 rows);
};
