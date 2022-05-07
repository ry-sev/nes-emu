#include "WindowManager.h"
#include "MenuWidget.h"
#include "RamWidget.h"
#include "StatusWidget.h"
#include "InstructionWidget.h"
#include "ScreenWidget.h"
#include "PatternTableWidget.h"
#include "NES.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

WindowManager::WindowManager(std::shared_ptr<NES> nes)
{
    m_nes = nes;
}

WindowManager::~WindowManager()
{
    if (m_widgets.size() > 0) {
        for (Widget* widget : m_widgets)
            delete widget;
    }
    destroy();
}

void WindowManager::init()
{
    // setup GLFW window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "GLFW could not be initiated\n";
        return;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // create window with graphics context    
    m_window = glfwCreateWindow(990, 720, "NES Emulator", NULL, NULL);
    if (m_window == NULL) {
        std::cerr << "GLFW window could not be created\n";
        return;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    // setup imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // setup imgui style
    ImGui::StyleColorsDark();

    // setup platform/renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    add_widget(new MenuWidget());
    add_widget(new ScreenWidget(m_nes->ppu()));
    add_widget(new RamWidget(m_nes->bus()));
    add_widget(new StatusWidget(m_nes->cpu()));
    add_widget(new PatternTableWidget(m_nes->ppu(), 0x00));
    add_widget(new PatternTableWidget(m_nes->ppu(), 0x01));
    add_widget(new InstructionWidget(m_nes));
}

void WindowManager::run()
{
    m_running = true;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(m_window) && m_running) {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (Widget* widget : m_widgets) {
            widget->render();
        }

        // Rendering
        ImGui::Render();
        int display_width, display_height;
        glfwGetFramebufferSize(m_window, &display_width, &display_height);
        glViewport(0, 0, display_width, display_height);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
    }
}

bool WindowManager::request_close()
{
    return glfwWindowShouldClose(m_window);
}

Widget* WindowManager::add_widget(Widget* widget)
{
    m_widgets.push_back(widget);
    return widget;
}

void WindowManager::destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
