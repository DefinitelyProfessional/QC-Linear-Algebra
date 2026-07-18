#include "headers/storage-utilities/storage-utilities.hpp"
#include "headers/math-core/function-definitions.hpp"
#include "headers/ui-utilities/stage-utilities.hpp"
#include "headers/ui-utilities/sandbox-manager.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <chrono>
#include <thread>
#include <string>

namespace fs = std::filesystem;

// Define target frame duration (1000 milliseconds / 60 FPS = 16.666 ms per frame)
const std::chrono::duration<double, std::milli> targetFrameTime(1000.0 / 60.0);

int main() {
    // Get the file path to the saved-data directory for global use ===========================================
    wchar_t buffer[MAX_PATH]; GetModuleFileNameW(NULL, buffer, MAX_PATH);
    const fs::path ROOT_DIR = fs::path(buffer).parent_path();
    const fs::path SAVED_DATA_DIR = ROOT_DIR / "saved-data";
    if (!fs::exists(SAVED_DATA_DIR)) {fs::create_directories(SAVED_DATA_DIR);}
    else if (!fs::is_directory(SAVED_DATA_DIR)) {// && fs::exist(SAVED_DATA_DIR)
        fs::remove(SAVED_DATA_DIR); fs::create_directories(SAVED_DATA_DIR); // Remove rogue n create directory.
    }

    // IMGUI SUBSYSTEMS INITIALIZATION ========================================================================
    GLFWwindow* window = STAGE::InitializeApplication(750, 1000, "QC Linear Algebra Sandbox Engine R.", ROOT_DIR);
    if (!window) {std::cerr << "Fatal Error: Failed to initialize application stages." << std::endl; return -1;}
    ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
    // ========================================================================================================

    // Load the Default Sandbox Session being "MAIN_sandbox.db" ===============================================
    SandboxSessionManager active_sandbox(SAVED_DATA_DIR, "MAIN_sandbox.db");
    // WindowManager to handle unified rendering of all windows ===============================================
    STAGE::WindowManager win_manager;
    // Register windows and get their pointers for event listeners ============================================
    SandboxManagerWindow* sandbox_manager = 
        win_manager.RegisterWindow<SandboxManagerWindow>(SAVED_DATA_DIR, active_sandbox.get_active_filename());
    sandbox_manager->Event_OnSelectSandbox = [&active_sandbox](std::string filename) {
        active_sandbox.switch_sandbox(filename);
    };
    sandbox_manager->Event_OnCreateSandbox = [](std::string filename) {
        std::cout << "CREATE : " << filename << std::endl;
    };

    // CORE IMGUI RENDER LOOP =================================================================================
    while (!glfwWindowShouldClose(window)) {
        // FOR FRAME CAPPING : Mark the exact time the frame started 
        auto frameStartTime = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        // -----------------------------------------------------------
        // Unified rendering of all window elements
        // -----------------------------------------------------------
        win_manager.RenderAll();
        // -----------------------------------------------------------
        // Finalize geometry and push to the GPU
        // -----------------------------------------------------------
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        // FOR FRAME CAPPING : Calculate how long the CPU took to draw the UI and do the math
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        auto timeSpentComputing = frameEndTime - frameStartTime;
        if (timeSpentComputing < targetFrameTime) {std::this_thread::sleep_for(targetFrameTime - timeSpentComputing);}
    }
    // ========================================================================================================

    // Deallocate everything and exit =========================================================================
    active_sandbox.save_sandbox(); // Save current data before exit
    STAGE::ShutdownApplication(window);
    // ========================================================================================================
    return 0;
}