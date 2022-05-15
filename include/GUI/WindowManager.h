#pragma once
#include <vector>
#include <memory>

struct GLFWwindow;
class Widget;
class NES;

class WindowManager
{

public:
    WindowManager(std::shared_ptr<NES> nes);
    ~WindowManager();

    void init();
    void run();
    void destroy();
    void handle_controller_input();
    bool request_close();

    Widget* add_widget(Widget* widget);

private:

    GLFWwindow* m_window = nullptr;
    std::vector<Widget*> m_widgets;
    bool m_running = false;
    std::shared_ptr<NES> m_nes;
};
