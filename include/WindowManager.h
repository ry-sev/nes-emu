#pragma once
#include <vector>
#include "Widget.h"
#include "Bus.h"
#include "CPU.h"

struct GLFWwindow;
class Widget;

class WindowManager
{

public:
    WindowManager(Bus* bus, CPU6502* cpu);
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
    Bus* m_bus;
    CPU6502* m_cpu;
};
