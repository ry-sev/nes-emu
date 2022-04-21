#pragma once
#include "imgui.h"

class Widget {

public:
    Widget() = default;
    virtual ~Widget() = default;
    virtual void render() = 0;

protected:
    bool begin(const char* widget_title);
    void end();

    ImGuiWindowFlags m_window_flags = 0;
    bool m_show = true;
};
