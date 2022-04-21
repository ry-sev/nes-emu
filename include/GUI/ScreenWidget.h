#pragma once
#include "Widget.h"

class ScreenWidget : public Widget
{

public:
    ScreenWidget();
    ~ScreenWidget() = default;

    void render() override;

private:
    ImDrawList* m_draw_list;
};
