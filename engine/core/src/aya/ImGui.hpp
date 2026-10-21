// ImGui
// for Aya
// by @mdolli

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "DataModel/DataModel.hpp"

namespace Aya
{
    class DebugImGui
    {
        public:
            DebugImGui(shared_ptr<Game> game);
            void draw();
            void resolutionChanged(unsigned int width, unsigned int height);
        private:
            void initialize();
            void drawFastClusterDebugging();
            void drawExplorer();

            ImGuiContext* context = nullptr;

            unsigned int width;
            unsigned int height;

            shared_ptr<Game> game;
    };
}