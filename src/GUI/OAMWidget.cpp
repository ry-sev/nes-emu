#include "OAMWidget.h"
#include "PPU.h"

OAMWidget::OAMWidget(std::shared_ptr<PPU> ppu)
{
    m_ppu = ppu;
}

void OAMWidget::render()
{
    if (!m_show)
        return;

    if (!begin("Object Attribute Entry")) {
        end();
        return;
    }

    m_flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_SizingStretchProp | 
        ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("oam contents", 4, m_flags)) {
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableSetupColumn("Entry");
        ImGui::TableSetupColumn("Position");
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Attr.");
        ImGui::TableHeadersRow();

        for (size_t row = 0; row < 13; row++) {
            auto entry = hex(row, 2);
            auto x_pos = std::to_string(m_ppu->oam_ptr[row * 4 + 3]);
            auto y_pos = std::to_string(m_ppu->oam_ptr[row * 4 + 0]);
            auto pos = "(" + x_pos + ", " + y_pos + ")";
            auto id = hex(m_ppu->oam_ptr[row * 4 + 1], 2);
            auto attr = hex(m_ppu->oam_ptr[row * 4 + 2], 2);

            ImGui::TableNextRow(0, 15.0f);
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(entry.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(pos.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(id.c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::TextUnformatted(attr.c_str());
        }
        ImGui::EndTable();
    }

    end();
}
