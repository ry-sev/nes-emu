#pragma once
#include <vector>
#include "Widget.h"

struct GLFWwindow;
class Widget;

class WindowManager
{

public:
    WindowManager();
    ~WindowManager();

    void init();
    void destroy();
    void run();
    bool request_close();

    Widget* add_widget(Widget* widget);

private:
    GLFWwindow* m_window = nullptr;
    std::vector<Widget*> m_widgets;
    bool m_running = false;
};
