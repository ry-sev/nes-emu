#pragma once
#include "Widget.h"

class ScreenWidget : public Widget
{

public:
    ScreenWidget();
    ~ScreenWidget() = default;

    void render() override;

private:
    u32 m_viewport_width = 0;
    u32 m_viewport_height = 0;
};
