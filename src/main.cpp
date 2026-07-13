#include "headers/ui-utilities/stage-utilities.hpp"
#include <iostream>

int main() {
    // Boot up imgui subsystems
    GLFWwindow* window = STAGE::InitializeApplication(1920, 1200, "QC Linear Algebra");
    if (!window) {
        std::cerr << "Fatal Error: Failed to initialize application stages." << std::endl;
        return -1;
    }
    ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);

    // 2. The Core Interactive Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Prepare fresh frames
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // -----------------------------------------------------------
        // ENGINE UI & EVENT LISTENERS
        // -----------------------------------------------------------
        
        ImGui::Begin("Matrix Operations");
        ImGui::Text("Welcome to the Linear Algebra Engine.");
        ImGui::Separator();
        
        // Immediate-mode event listening:
        // This button function returns true ONLY on the exact frame the user clicks it.
        if (ImGui::Button("Perform Identity Matrix Check")) {
            std::cout << "[Engine] Executing mathematical operation..." << std::endl;
            // You will eventually call your math-core functions here
        }
        ImGui::End();

        ImGui::ShowDemoWindow();

        // -----------------------------------------------------------
        
        // Finalize geometry and push to the GPU
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Deallocate everything and exit
    STAGE::ShutdownApplication(window);

    return 0;
}