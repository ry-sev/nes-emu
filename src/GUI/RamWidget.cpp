#include "RamWidget.h"
#include "Bus.h"
#include <sstream>

RamWidget::RamWidget(Bus* bus)
{
    m_bus = bus;
}

void RamWidget::render()
{
    if (!m_show)
        return;

    //m_window_flags |= ImGuiWindowFlags_NoResize;

    if (!begin("RAM Dump")) {
        end();
        return;
    }

    m_flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_ScrollY;

    create_table("ram_table_2", 0x8000, 32);
    
    end();
}

void RamWidget::create_table(const char* table_name, u16 address, u32 rows)
{
    // TODO: Highlight rows on hover
    
    if (ImGui::BeginTable(table_name, 3, m_flags)) {

        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Hex");
        ImGui::TableSetupColumn("ASCII");
        ImGui::TableHeadersRow();

        for (size_t row = 0; row < rows; row++) {

            ImGui::TableNextRow();
            auto offset = hex(address, 4);

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(offset.c_str());

            std::stringstream hex_stream;
            std::stringstream ascii_stream;

            for (size_t i = 0; i < 16; i++) {
                auto value = hex(m_bus->cpu_read(address), 2);
                hex_stream << " " << value;
                ascii_stream << "" << hex_to_ascii(value);
                address++;
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(hex_stream.str().c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(ascii_stream.str().c_str());
        }
        ImGui::EndTable();
    }
}
