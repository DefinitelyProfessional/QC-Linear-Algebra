#include "headers/ui-utilities/general-utilities.hpp"
#include "headers/ui-utilities/stage-utilities.hpp"
#include "headers/math-core/function-definitions.hpp"
#include "headers/storage-utilities/storage-utilities.hpp"

#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

// Define target frame duration (1000 milliseconds / 60 FPS = 16.666 ms per frame)
const std::chrono::duration<double, std::milli> targetFrameTime(1000.0 / 60.0);

int main() {
    // IMGUI SUBSYSTEMS INITIALIZATION ========================================================================
    GLFWwindow* window = STAGE::InitializeApplication(750, 1000, "QC Linear Algebra Sandbox Engine R.");
    if (!window) {std::cerr << "Fatal Error: Failed to initialize application stages." << std::endl; return -1;}
    ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
    // ========================================================================================================

    // Get the file path to the saved-data directory for global use ===========================================
    const fs::path SAVED_DATA_DIR = fs::current_path() / "saved-data";
    if (!fs::exists(SAVED_DATA_DIR)) {fs::create_directories(SAVED_DATA_DIR);}
    // WindowManager to handle unified rendering of all windows ===============================================
    STAGE::WindowManager win_manager;
    // Register windows and get their pointers for event listeners ============================================
    UI::SandboxManagerWindow* sandbox_manager = 
        win_manager.RegisterWindow<UI::SandboxManagerWindow>(SAVED_DATA_DIR);
    sandbox_manager->Event_OnSelectSandbox = [](std::string filename) {
        std::cout << "SELECTED" << filename << std::endl; // placeholder
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
    STAGE::ShutdownApplication(window);
    // ========================================================================================================
    return 0;
}