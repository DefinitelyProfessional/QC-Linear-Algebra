#pragma once

// Include for UIWindow class
#include "ui-utilities/general-utilities.hpp"

#include "imgui.h"

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

// Forward declare GLFW
struct GLFWwindow;

// Namespace for all STAGE related functionalities include UI initialization and shutdown and more
namespace STAGE {
    // Unified class to manage rendering all windows with imgui
    class WindowManager {
    private:
        std::vector<std::unique_ptr<UIWindow>> windows_registry;

    public:
        // Register a new window dynamically into the ecosystem
        template <typename T, typename... Args> T* RegisterWindow(Args&&... args) {
            // Lvalues : PERSISTENT MEMORY address with an IDENTITY (COPY!)
            // Rvalues : TEMPORARY OBJECT that DOESN'T have a persistent memory address (MOVE!)
            //  Allocate and return unique_ptr memory for the window objects
            auto window = std::make_unique<T>(std::forward<Args>(args)...);
            // Store a non-owning pointer to the window object for callbacks at main
            T* rawPtr = window.get();
            // window no longer needed locally, transfer mem loc ownership to windows_registry
            windows_registry.push_back(std::move(window));
            return rawPtr; // Return pointer so main can bind callbacks immediately
        }

        // Single unified dispatch call to render every single registered component
        void RenderAll();

        // Optional: Search for a window dynamically by its ImGui string ID
        // UIWindow* FindWindow(const std::string& name);
    };

    // Encapsulates the complete boot sequence for GLFW, OpenGL, and ImGui
    GLFWwindow* InitializeApplication(int width, int height, const char* TITLE, const std::filesystem::path& ROOT);

    // Encapsulates the complete beginning of the render loop
    void StartRenderLoop();

    // Encapsulates the complete end of the render loop
    void EndRenderLoop(GLFWwindow* window, const ImVec4& clear_color);

    // Encapsulates the complete memory cleanup n shutdown sequence
    void ShutdownApplication(GLFWwindow* window);
}