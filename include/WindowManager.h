#pragma once
#include <vector>

struct GLFWwindow;
class Widget;
class Bus;
class CPU6502;

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
