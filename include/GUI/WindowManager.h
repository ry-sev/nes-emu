#pragma once
#include <vector>

struct GLFWwindow;
class Widget;
class NES;

class WindowManager
{

public:
    WindowManager(NES* nes);
    ~WindowManager();

    void init();
    void run();
    void destroy();
    bool request_close();

    Widget* add_widget(Widget* widget);

private:

    GLFWwindow* m_window = nullptr;
    std::vector<Widget*> m_widgets;
    bool m_running = false;
    NES* m_nes;
};
