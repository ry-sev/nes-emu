#pragma once
#include "Widget.h"
#include "Image.h"
#include <memory>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

class PPU;

class ScreenWidget : public Widget
{

public:
    ScreenWidget(std::shared_ptr<PPU> ppu);
    ~ScreenWidget() = default;

    void render() override;

private:
    std::shared_ptr<PPU> m_ppu;
    Image m_screen = Image(SCREEN_WIDTH, SCREEN_HEIGHT);
};
