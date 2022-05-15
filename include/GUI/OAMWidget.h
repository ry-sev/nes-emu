#include "Widget.h"
#include <memory>

class PPU;

class OAMWidget : public Widget {

public:
    OAMWidget(std::shared_ptr<PPU> ppu);
    ~OAMWidget() = default;
    void render() override;

private:
    std::shared_ptr<PPU> m_ppu;
    ImGuiTabBarFlags m_flags;
};
