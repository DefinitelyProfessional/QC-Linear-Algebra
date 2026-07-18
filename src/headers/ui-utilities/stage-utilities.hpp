#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

// Expose GLFW's native Win32 functions
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// Include the resource ID definition
#include "icon-resource.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Base class for windows based UI
class UIWindow {
protected:
    std::string window_name;
    bool is_open;

public:
    UIWindow(const std::string& name, bool startOpen = true)
        : window_name(name), is_open(startOpen) {}

    virtual ~UIWindow() = default;

    // Pure virtual function: Every window MUST implement its layout logic here
    virtual void Render() = 0;

    // Standard getters/setters for window visibility
    bool IsOpen() const { return is_open; }
    void SetOpen(bool open) { is_open = open; }
    const std::string& GetName() const { return window_name; }
};


// Namespace for all STAGE related functionalities include UI initialization and shutdown and more
namespace STAGE {
    // Unified class to manage rendering all windows with imgui ===============================================
    class WindowManager {
    private:
        std::vector<std::unique_ptr<UIWindow>> windows_registry;
    public:
        // Register a new window dynamically into the ecosystem
        template <typename T, typename... Args> T* RegisterWindow(Args&&... args) {
            // Create the derived window instance safely
            auto window = std::make_unique<T>(std::forward<Args>(args)...);
            T* rawPtr = window.get();
            windows_registry.push_back(std::move(window));
            return rawPtr; // Return pointer so main can bind callbacks immediately
        }

        // Single unified dispatch call to render every single registered component
        void RenderAll() {
            for (const auto& window : windows_registry) {
                if (window->IsOpen()) {window->Render();}
            }
        }

        // Optional: Search for a window dynamically by its ImGui string ID
        UIWindow* FindWindow(const std::string& name) {
            for (const auto& window : windows_registry) {
                if (window->GetName() == name) return window.get();
            }
            return nullptr;
        }
    };

    // Internal robust GLFW error callback ====================================================================
    inline void glfw_error_callback(int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }

    // Encapsulates the complete boot sequence for GLFW, OpenGL, and ImGui ====================================
    inline GLFWwindow* InitializeApplication(int width, int height, const char* title) {
        // GLFW Setup
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) return nullptr;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Window Creation
        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable VSync

        // Apply the native icon
        HWND hwnd = glfwGetWin32Window(window);
        if (hwnd) {
            HINSTANCE hInst = GetModuleHandle(NULL);
            // Load the large icon for Taskbar / Alt+Tab
            HICON hIconBig = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 
                GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
            // Load the small icon for the Window Title Bar
            HICON hIconSmall = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APP_ICON), IMAGE_ICON, 
                GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
            // Send messages to the Win32 window to update the icons
            if (hIconBig) SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
            if (hIconSmall) SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
        }

        // ImGui Context Setup
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::StyleColorsDark();

        // ImGui Backend Binding
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // Extract and locate the imgui.ini to portable dist directory
        wchar_t buffer[MAX_PATH];
        GetModuleFileNameW(NULL, buffer, MAX_PATH);
        std::filesystem::path exeDir = std::filesystem::path(buffer).parent_path();
        std::filesystem::path iniPath = exeDir / "imgui.ini";
        static std::string persistentIniPath = iniPath.string();
        io.IniFilename = persistentIniPath.c_str();
        
        return window;
    }

    // Encapsulates the complete memory cleanup sequence ======================================================
    inline void ShutdownApplication(GLFWwindow* window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

}