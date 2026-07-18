#pragma once

#include "imgui.h"

// for UIWindow 
#include "stage-utilities.hpp"

#include <filesystem>
#include <functional>
#include <string>

namespace fs = std::filesystem;

namespace UI {
    // Sandbox selector window
    class SandboxManagerWindow : public UIWindow {
    private:
        fs::path saved_data_dir;
    
    public:
        // Event Listeners to be defined in main
        std::function<void(std::string)> Event_OnSelectSandbox;

        // Constructor simply sets UIWindow window_name and directory filepath
        SandboxManagerWindow(const fs::path& data_dir) : 
            UIWindow("SandboxManager"), saved_data_dir(data_dir) {}

        // Render function definition
        void Render() override {
            if (!is_open) return; // render control

        }
    };
    // class VectorEntryWindow : public UIWindow {
    // private:
    //     VectorManager* m_VectorManager;
    //     int m_Dimensions;
    //     std::vector<float> m_Entries;

    // public:
    //     VectorEntryWindow(int dimensions, VectorManager* vectorMgr) 
    //         : UIWindow("Populate Vector Data"), m_VectorManager(vectorMgr), m_Dimensions(dimensions) {
    //         m_Entries.resize(dimensions, 0.0f); // Pre-allocate entries with zeros
    //     }

    //     void Render() override {
    //         if (!IsOpen) return;

    //         ImGui::Begin(Name.c_str(), &IsOpen);
    //         ImGui::Text("Enter values for your %d-dimensional vector:", m_Dimensions);
    //         ImGui::Separator();

    //         // Dynamically generate input fields based on requested dimensions
    //         for (int i = 0; i < m_Dimensions; ++i) {
    //             std::string label = "Entry [" + std::to_string(i) + "]";
    //             ImGui::InputFloat(label.c_str(), &m_Entries[i]);
    //         }

    //         ImGui::Separator();
    //         if (ImGui::Button("Commit Vector to Memory")) {
    //             m_VectorManager->SaveVector("User_Vector_" + std::to_string(m_Dimensions), m_Entries);
    //             IsOpen = false; // Close this window when done!
    //         }

    //         ImGui::End();
    //     }
    // };
    
    // class VectorCreationWindow : public UIWindow {
    // private:
    //     WindowManager* m_WinManager;
    //     VectorManager* m_VectorManager;
    //     char m_DimBuffer[64] = "";
    //     std::string m_ErrorMessage = "";

    // public:
    //     VectorCreationWindow(WindowManager* winMgr, VectorManager* vecMgr) 
    //         : UIWindow("Create Classical Vector"), m_WinManager(winMgr), m_VectorManager(vecMgr) {}

    //     void Render() override {
    //         ImGui::Begin(Name.c_str());

    //         ImGui::Text("Specify Vector Dimensions:");
    //         // Input text filtered to decimal digits only
    //         ImGui::InputText("Dimensions", m_DimBuffer, sizeof(m_DimBuffer), ImGuiInputTextFlags_CharsDecimal);

    //         if (ImGui::Button("Initialize Vector Space")) {
    //             std::string inputStr(m_DimBuffer);
                
    //             // Validation Logic
    //             if (inputStr.empty()) {
    //                 m_ErrorMessage = "Error: Input cannot be empty.";
    //             } else {
    //                 try {
    //                     int dims = std::stoi(inputStr);
    //                     if (dims <= 0) {
    //                         m_ErrorMessage = "Error: Dimensions must be strictly positive (> 0).";
    //                     } else {
    //                         // Success! Clear errors and spawn the entry window dynamically
    //                         m_ErrorMessage = "";
    //                         auto entryWin = std::make_shared<VectorEntryWindow>(dims, m_VectorManager);
    //                         m_WinManager->AddWindow(entryWin);
    //                     }
    //                 } catch (...) {
    //                     m_ErrorMessage = "Error: Failed to convert input to integer.";
    //                 }
    //             }
    //         }

    //         // Display error message in red if validation fails
    //         if (!m_ErrorMessage.empty()) {
    //             ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", m_ErrorMessage.c_str());
    //         }

    //         ImGui::End();
    //     }
    // };

    // class UI_ModuleName_Controller {
    // public:
    //     // =========================================================================
    //     // 1. EVENT LISTENERS (The "Slots")
    //     // These are placeholders for functions. The UI doesn't know what these do; 
    //     // it just knows to call them when a specific ImGui widget is interacted with.
    //     // =========================================================================
        
    //     // A basic trigger event (no data passed)
    //     std::function<void()> Event_OnSimpleActionTriggered;
        
    //     // An event that passes a specific value back to the engine
    //     std::function<void(int)> Event_OnDataActionTriggered;
        
    //     // An event that triggers immediately when a value changes state
    //     std::function<void(bool)> Event_OnToggleStateChanged;

    //     // =========================================================================
    //     // 2. INTERNAL UI STATE (The "Memory")
    //     // Dear ImGui requires physical memory addresses to bind its interactive 
    //     // widgets to. These variables retain your UI's state between frames.
    //     // =========================================================================
        
    //     bool State_IsToggled = false;
    //     int State_SliderValue = 50;
    //     float State_FloatValue = 1.0f;
    //     char State_TextBuffer[256] = "Default Text"; // ImGui uses C-style char arrays by default

    //     // =========================================================================
    //     // 3. THE RENDER LOOP (The "Presentation")
    //     // This is the only function you call inside your main window loop.
    //     // =========================================================================
        
    //     void Render() {
    //         // Create the window panel
    //         ImGui::Begin("Placeholder_Panel_Name");

    //         // --- WIDGET 1: A Standard Button ---
    //         // ImGui::Button returns true ONLY on the exact frame the user clicks it.
    //         if (ImGui::Button("Trigger_Simple_Action_Label")) {
    //             if (Event_OnSimpleActionTriggered) { // Check if the engine bound a function
    //                 Event_OnSimpleActionTriggered(); // Fire the event!
    //             }
    //         }

    //         ImGui::Separator();

    //         // --- WIDGET 2: A Checkbox (Toggle) ---
    //         // We pass the memory address (&) of our state boolean. 
    //         // ImGui automatically flips this boolean when clicked.
    //         if (ImGui::Checkbox("Toggle_Setting_Label", &State_IsToggled)) {
    //             // We can detect the exact frame it changed and notify the engine
    //             if (Event_OnToggleStateChanged) {
    //                 Event_OnToggleStateChanged(State_IsToggled);
    //             }
    //         }

    //         // --- WIDGET 3: A Slider (Continuous Data) ---
    //         // Modifies the integer directly. We define the min (0) and max (100) range.
    //         ImGui::SliderInt("Adjust_Value_Label", &State_SliderValue, 0, 100);

    //         // --- WIDGET 4: Text Input ---
    //         // Binds to our char array. The 'sizeof' prevents buffer overflow crashes.
    //         ImGui::InputText("Text_Input_Label", State_TextBuffer, sizeof(State_TextBuffer));

    //         // --- WIDGET 5: A Button that uses complex state data ---
    //         if (ImGui::Button("Submit_Data_Label")) {
    //             if (Event_OnDataActionTriggered) {
    //                 // Pass the current slider state out to the engine
    //                 Event_OnDataActionTriggered(State_SliderValue); 
    //             }
    //         }

    //         ImGui::End();
    //     }
    // };

    //     // A struct to hold the mathematical result data
    // struct MathResultPayload {
    //     int finalMatrixSize = 0;
    //     std::string computationLog = "";
    //     // You would include your actual Matrix data structures here (e.g., std::vector<float>)
    // };

    // class MatrixEngineUI {
    // public:
    //     // =========================================================================
    //     // THREADING & SYNCHRONIZATION STATE
    //     // =========================================================================
        
    //     std::atomic<bool> isComputing{false};        // Hardware-safe flag to track thread status
    //     std::atomic<bool> hasNewResult{false};       // Hardware-safe flag to notify the UI
        
    //     std::mutex dataMutex;                        // The lock protecting the payload
    //     MathResultPayload latestResult;              // The protected data payload

    //     int State_InputGridSize = 3;                 // UI Input State

    //     // Event Listener for the dispatch
    //     std::function<void(int)> Event_DispatchComputation;

    //     // =========================================================================
    //     // THE RENDER LOOP
    //     // =========================================================================
        
    //     void Render() {
    //         ImGui::Begin("Linear Algebra Dispatcher");

    //         // 1. The Input Section
    //         // Disable inputs while computing so the user can't spam the background thread
    //         ImGui::BeginDisabled(isComputing.load()); 
    //         ImGui::SliderInt("Matrix Size", &State_InputGridSize, 2, 100);

    //         if (ImGui::Button("Begin 5-Second Computation")) {
    //             if (Event_DispatchComputation) {
    //                 // Set the flag immediately so the UI locks up the button on the very next frame
    //                 isComputing.store(true); 
    //                 Event_DispatchComputation(State_InputGridSize);
    //             }
    //         }
    //         ImGui::EndDisabled();

    //         // Show a loading indicator if the thread is running
    //         if (isComputing.load()) {
    //             ImGui::SameLine();
    //             ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Processing on background thread...");
    //         }

    //         ImGui::Separator();

    //         // 2. The Output Retrieval Section
    //         // The UI polls this atomic flag at 60+ FPS. It costs almost zero CPU cycles.
    //         if (hasNewResult.load()) {
                
    //             // The thread flagged completion! Now we lock the mutex to safely copy the data.
    //             // std::lock_guard automatically locks the mutex and unlocks it when it goes out of scope.
    //             std::lock_guard<std::mutex> lock(dataMutex);
                
    //             // Render the protected data
    //             ImGui::Text("Computation Complete!");
    //             ImGui::Text("Resulting Matrix Size: %d x %d", latestResult.finalMatrixSize, latestResult.finalMatrixSize);
    //             ImGui::TextWrapped("Log: %s", latestResult.computationLog.c_str());
                
    //             // Note: We do NOT set hasNewResult to false here, because we want 
    //             // the result to stay on screen until the next computation starts.
    //         }

    //         ImGui::End();
    //     }
    // };

}