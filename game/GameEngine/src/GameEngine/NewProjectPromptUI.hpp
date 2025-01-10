#pragma once
#include "Syris/layers/Layer.hpp"
#include "Syris/include/imgui.h"
#include "EngineState.hpp"
#include "Syris/include/windows.h"
#include "Syris/utils/imgui/styles.h"
#include <ShObjIdl.h>

namespace GameEngine{
    class NewProjectPromptUI : public Syris::Layer{
    public:
        NewProjectPromptUI(EngineState &state) : m_state(state)
        {
        }
        ~NewProjectPromptUI() {};

        void on_update(const Syris::engine_time::Time &time) override
        {
            ImGui::Begin("NewProjectPrompUi");
            {
                if (ImGui::Button("Select project path"))
                    m_current_path = get_user_path();
                ImGui::Text("Current path: %s", Syris::WStringToUTF8(m_current_path).c_str());
                char buffer[100] = "";
                ImGui::Text("Project name");
                ImGui::PushID("InputField"); // Push a unique ID to avoid requiring a label
                ImGui::InputText("", buffer, sizeof(buffer));
                ImGui::PopID();
                //ImGui::InputText("##Project name", buffer, sizeof(buffer));
                m_project_name = std::string(buffer, strlen(buffer));
                if (ImGui::Button("Begin Project"))
                    m_state.begin_new_project(m_project_name);
            }
            ImGui::End();
        }
        bool on_event(Syris::Event *event) override
        {
            return false;
        }

        std::wstring get_user_path()
        {
            // Initialize COM library
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            if (FAILED(hr))
                std::wcerr << L"Failed to initialize COM library." << std::endl;

            std::wstring folderPath;

            // Create the File Open Dialog object
            IFileDialog *pFileDialog = nullptr;
            hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));
            if (SUCCEEDED(hr))
            {
                // Set options for picking folders
                DWORD dwOptions;
                if (SUCCEEDED(pFileDialog->GetOptions(&dwOptions)))
                {
                    pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
                }

                // Show the dialog
                hr = pFileDialog->Show(nullptr);
                if (SUCCEEDED(hr))
                {
                    // Get the result
                    IShellItem *pItem = nullptr;
                    hr = pFileDialog->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFolderPath = nullptr;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                        if (SUCCEEDED(hr))
                        {
                            folderPath = pszFolderPath;
                            CoTaskMemFree(pszFolderPath);
                        }
                        pItem->Release();
                    }
                }
                pFileDialog->Release();
            }
            else
            {
                std::wcerr << L"Failed to create FileOpenDialog instance." << std::endl;
            }

            // Uninitialize COM library
            CoUninitialize();
            return folderPath;
        }

    private:
        std::string m_project_name = "Default";
        std::wstring m_current_path = L"None";
        EngineState &m_state;
    };
}