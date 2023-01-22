#include "gui.h"
td::gui::gui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); // (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    if(m_style) ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
void td::gui::render() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //rendering
    for(auto& w: m_windows){
        w->create();
    }
    ImGui::Render();
}
void td::gui::draw() const
{
     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static bool g_show = true;
void td::test_win::create()
{
    ImGui::ShowDemoWindow(&g_show);
}
td::gui::~gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
