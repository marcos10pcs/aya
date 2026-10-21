// ImGui
// for Aya
// by @mdolli
#include "ImGui.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>

namespace Aya
{
    DebugImGui::DebugImGui()
    {
        if(!context)
            initialize();
    }

    DebugImGui::~DebugImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    void DebugImGui::initialize()
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplOpenGL3_Init();
    }

    void DebugImGui::draw()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        
        drawFastClusterDebugging();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void DebugImGui::resolutionChanged(unsigned int newWidth, unsigned int newHeight)
    {
        width = newWidth;
        height = newHeight;
    }

    void DebugImGui::drawFastClusterDebugging()
    {
        ImGui::Begin("FastClusters");
        ImGui::Text("This will display FastCluster debugging info");
        ImGui::End();
    }
}